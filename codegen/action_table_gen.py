from dataclasses import dataclass


# Action table values.
@dataclass(frozen=True)
class Shift:
    pass

@dataclass(frozen=True)
class Reduce:
    production: int

@dataclass(frozen=True)
class Accept:
    pass

@dataclass(frozen=True)
class Conflict:
    pass

Action = Shift | Reduce | Accept | Conflict | None


# Production rules of language grammar
GRAMMAR: list[tuple[str, list[str]]] = [
    ("Program", ["Block"]),

    ("Block", ["S"]),
    ("Block", ["S", "Block"]),

    ("S", ["int", "x"]),
    ("S", ["x", "=", "AExp"]),
    ("S", ["assert", "BExp"]),
    ("S", ["if", "BExp", "then", "Block", "end"]),
    ("S", ["if", "BExp", "then", "Block", "else", "Block", "end"]),
    ("S", ["while", "BExp", "do", "Block", "end"]),

    ("AExp", ["AExp", "+", "AExp"]),
    ("AExp", ["AExp", "*", "AExp"]),
    ("AExp", ["AExp", "-", "AExp"]),
    ("AExp", ["(", "AExp", ")"]),
    ("AExp", ["x"]),
    ("AExp", ["const"]),

    ("BExp", ["BExp", "and", "BExp"]),
    ("BExp", ["BExp", "or", "BExp"]),
    ("BExp", ["not", "BExp"]),
    ("BExp", ["(", "BExp", ")"]),
    ("BExp", ["AExp", "<", "AExp"]),
    ("BExp", ["AExp", ">", "AExp"]),
    ("BExp", ["AExp", "==", "AExp"]),
    ("BExp", ["AExp", "!=", "AExp"]),
    ("BExp", ["true"]),
    ("BExp", ["false"]),
]
# Map grammar-terminal -> TokenType enum name.
TERMINAL_TO_TOKEN: dict[str, str] = {
    "assert":  "ASSERT",
    "if":      "IF",
    "then":    "THEN",
    "else":    "ELSE",
    "while":   "WHILE",
    "do":      "DO",
    "end":     "END",
    "int":     "INT",
    "const":   "CONSTANT",
    "true":    "TRUE",
    "false":   "FALSE",
    "x":       "IDENTIFIER",
    "=":       "EQ",
    "+":       "PLUS",
    "-":       "MINUS",
    "*":       "MUL",
    "==":      "EQEQ",
    "!=":      "NEQ",
    "<":       "LT",
    ">":       "GT",
    "and":     "AND",
    "or":      "OR",
    "not":     "NOT",
    "(":       "LPAREN",
    ")":       "RPAREN",
    "eof":     "END_OF_FILE",
}

TERMINALS: list[str] = list(TERMINAL_TO_TOKEN.keys())
NONTERMINALS: list[str] = ["Program", "S", "Block", "AExp", "BExp"]

# Operator precedence and associativity for shift-reduce conflict resolution.
PRECEDENCE: dict[str, tuple[int, str]] = {
    "or":  (0, "left"),
    "and": (1, "left"),
    "not": (2, "right"),
    "+":   (3, "left"),
    "-":   (3, "left"),
    "*":   (4, "left"),
}

# Fallback for conflicts that need to be resolved manually
CONFLIC_RESOLUTION: dict[int, dict[str, Action]] = {
    # Example: 42: {"else": Shift(), "end": Reduce(7)},
}



START_SYMBOL = "Program"

# Variable that will hold constructed LR0 sets
lr_sets : list[set[tuple[int,int]]] = []

goto_table: list[dict[str, int]] = []

action_table: list[list[Action]] = []
expanded_goto_table: list[list[int]] = []

# for debugging and vorbose output purposes
obtained_from: list[str] = []

def goto(target_index: int, symbol: str):
    if symbol in goto_table[target_index]:
        return goto_table[target_index][symbol]
    # Reaching this means goto table for that symbol was not yet constructed
    # Or invalid symbol/ does not occur in any production as next symbol
    new_lr_set = set()
    #Construct goto
    for production_index, next_index in lr_sets[target_index]:
        _, rhs = GRAMMAR[production_index]
        # skip is dot is at the end
        if next_index >= len(rhs):
            continue 
        next_symbol = rhs[next_index]
        #skip if not the symbol we want to goto
        if not next_symbol == symbol:
            continue
        new_lr_set.add(((production_index, next_index+1)))
    
    if not new_lr_set:
      return None
    
    new_lr_set = closure(new_lr_set)
    try:
        index = lr_sets.index(new_lr_set)
    except ValueError:
        lr_sets.append(new_lr_set)
        parent_path = obtained_from[target_index]
        obtained_from.append(f"{parent_path} {symbol}".strip())
        goto_table.append({})
        index = len(lr_sets) - 1

    goto_table[target_index][symbol] = index
    return index
    

def closure(lr_set: set[tuple[int,int]]):
    #print(f"Closure of {lr_set}:")
    lr_set = set(lr_set)
    worklist = list(lr_set)

    while len(worklist)>0:
        production_index, next_index = worklist.pop()
        _, rhs = GRAMMAR[production_index]
        # skip is dot is at the end
        if next_index >= len(rhs):
            continue 
        next_symbol = rhs[next_index]
         # skip is next is a terminal (not uppercase)
        if not 'A' <= next_symbol[0] <= 'Z':
            continue
        for i,(nonterminal, _) in enumerate(GRAMMAR):
            if nonterminal == next_symbol and (i,0) not in lr_set:
                worklist.append((i,0))
                lr_set.add((i,0))
    #print(f"--> {lr_set}")
    return lr_set

def production_op(prod_idx: int):
    _, rhs = GRAMMAR[prod_idx]
    for sym in reversed(rhs):
        if sym in TERMINALS:
            return sym
    return None


def resolve_by_precedence(reduce_prod: int, lookahead: str):
    op = production_op(reduce_prod)
    if op is None or op not in PRECEDENCE or lookahead not in PRECEDENCE:
        return Conflict()

    op_level, op_assoc = PRECEDENCE[op]
    look_level, _      = PRECEDENCE[lookahead]

    if look_level > op_level:
        return Shift()
    if look_level < op_level:
        return Reduce(reduce_prod)
    # Equal precedence — use associativity of the reduce operator
    if op_assoc == "left":
        return Reduce(reduce_prod)
    if op_assoc == "right":
        return Shift()
    if op_assoc == "nonassoc":
        return None
    return Conflict()


def manual_resolution(lr_set_index: int, symbol: str):
    return CONFLIC_RESOLUTION.get(lr_set_index, {}).get(symbol)


def action(lr_set_index: int, symbol: str):
    if symbol not in TERMINALS:
        return None

    # find out if shift non-terminal or reduce
    shift_items = []
    reduce_items = []
    for (production_index, next_index) in lr_sets[lr_set_index]:
        lhs, rhs = GRAMMAR[production_index]
        # check is dot is at the end
        if next_index >= len(rhs):
            # SLR(1) filter: only reduce when lookahead is in FOLLOW(lhs)
            if symbol in FOLLOW[lhs]:
                reduce_items.append((production_index, next_index))
            continue
        next_symbol = rhs[next_index]
        if next_symbol == symbol:
            #terminal, so shift!
            shift_items.append((production_index, next_index))

    if len(reduce_items) > 1:
        # Reduce-reduce conflict — fall back to manual resolution
        return manual_resolution(lr_set_index, symbol) or Conflict()

    # Shift-reduce conflict — try precedence, then manual fallback
    if len(reduce_items) == 1 and len(shift_items) > 0:
        result = resolve_by_precedence(reduce_items[0][0], symbol)
        if isinstance(result, Conflict):
            return manual_resolution(lr_set_index, symbol) or Conflict()
        return result

    if len(reduce_items) == 1:
        prod_idx = reduce_items[0][0]
        if prod_idx == 0 and symbol == "eof":
            return Accept()
        return Reduce(prod_idx)
    if len(shift_items) > 0:
        return Shift()
    return None

#Implement way to provide the info for resolving the conflicts.

def gen_act_table():
    dim_x = len(TERMINALS)
    dim_y = len(lr_sets)
    table = [[None] * dim_x for _ in range(dim_y)]
    for i,_ in enumerate(lr_sets):
         for j,sym in enumerate(TERMINALS):
             table[i][j] = action(i,sym)
    return table

def gen_goto_table():
    # Column layout: terminals first (TokenType ordinal order), then nonterminals.
    columns = TERMINALS + NONTERMINALS
    dim_x = len(columns)
    dim_y = len(lr_sets)
    table = [[None] * dim_x for _ in range(dim_y)]
    for i in range(dim_y):
        for j, sym in enumerate(columns):
            table[i][j] = goto(i, sym)
    return table

def encode_action(action: Action):
    match  action:
        case None: return 0 #represents Error
        case Shift(): return 1
        case Accept(): return 2
        case Reduce(production=p): return -p
        case Conflict(): raise ValueError("Unresovled conflict")

def encode_act_table(table: list[list[Action]]):
    encoded_table = [[encode_action(action) for action in row] for row in table]
    return encoded_table

def get_conflicts(action_table: list[list[Action]]):
    conflicting_indices = []
    for i,row in enumerate(action_table):
        if Conflict() in row:
            conflicting_indices.append(i)
    return conflicting_indices


def gather_all_symbols():
    symbols = []
    seen = set()
    for (lhs,rhs) in GRAMMAR:
        seen.add(lhs)
        seen.update(rhs)
    symbols[:] = sorted(seen)
    return []

def construct_IR0_sets():
    ## initial IR0 set
    lr_sets.append(closure({(0,0)}))
    obtained_from.append("")
    goto_table.append({})

    current_index = 0
    while current_index < len(lr_sets):
        next_symbols = set()
        for production_index, next_index in lr_sets[current_index]:
            _, rhs = GRAMMAR[production_index]
            # skip is dot is at the end
            if next_index >= len(rhs):
                continue 
            next_symbols.add(rhs[next_index])
        for sym in sorted(next_symbols):
            goto(current_index, sym)
        current_index += 1

FIRST: dict[str, set[str]] = {}
FOLLOW: dict[str, set[str]] = {}


def compute_first():
    FIRST.clear()
    for nt in NONTERMINALS:
        FIRST[nt] = set()

    changed = True
    while changed:
        changed = False
        for lhs, rhs in GRAMMAR:
            sym = rhs[0]
            if sym in NONTERMINALS:
                added = FIRST[sym] - FIRST[lhs]
                if added:
                    FIRST[lhs] |= added
                    changed = True
            else:  # terminal
                if sym not in FIRST[lhs]:
                    FIRST[lhs].add(sym)
                    changed = True


def compute_lookahead():
    FOLLOW.clear()
    for nt in NONTERMINALS:
        FOLLOW[nt] = set()
    FOLLOW[START_SYMBOL].add("eof")

    changed = True
    while changed:
        changed = False
        for lhs, rhs in GRAMMAR:
            for i, sym in enumerate(rhs):
                if sym not in NONTERMINALS:
                    continue
                beta = rhs[i + 1:]
                if not beta:
                    # case 3: nonterminal at end of production
                    added = FOLLOW[lhs] - FOLLOW[sym]
                else:
                    next_sym = beta[0]
                    if next_sym in NONTERMINALS:
                        # case 2b: followed by a nonterminal
                        added = FIRST[next_sym] - FOLLOW[sym]
                    else:
                        # case 2a: followed by a terminal
                        added = {next_sym} - FOLLOW[sym]
                if added:
                    FOLLOW[sym] |= added
                    changed = True

def format_item(prod_idx: int, dot: int) -> str:
    lhs, rhs = GRAMMAR[prod_idx]
    rhs_with_dot = rhs[:dot] + ["."] + rhs[dot:]
    return f"{lhs} -> {' '.join(rhs_with_dot)}"


def dump_states(sets : list[set[tuple[int,int]]], file=None):
    print("\n" + "=" * 70, file=file)
    print(f"Total states: {len(sets)}", file=file)
    print("=" * 70, file=file)
    for i, items in enumerate(sets):
        path = obtained_from[i] if obtained_from[i] else "eps"
        print(f"\nI{i} ({path}):", file=file)
        for prod_idx, dot in sorted(items):
            print(f"    [{format_item(prod_idx, dot)}]", file=file)
        if goto_table[i]:
            transitions = ", ".join(f"{sym} -> I{j}" for sym, j in sorted(goto_table[i].items()))
            print(f"  goto: {transitions}", file=file)



def main():
    gather_all_symbols()
    construct_IR0_sets()
    compute_first()
    compute_lookahead()
    #dump_states(lr_sets)
    action_table[:] = gen_act_table()
    expanded_goto_table[:] = gen_goto_table()
    print(f"Conflicts: {get_conflicts(action_table)}")
    
    for i in get_conflicts(action_table):
        path = obtained_from[i] if obtained_from[i] else "eps"
        print(f"\nI{i} ({path}):")
        for prod_idx, dot in sorted(lr_sets[i]):
            print(f"    [{format_item(prod_idx, dot)}]")
        if goto_table[i]:
            transitions = ", ".join(f"{sym} -> I{j}" for sym, j in sorted(goto_table[i].items()))
            print(f"  goto: {transitions}")

    

    with open("../src/parser/lr_sets.txt", "w") as f:
        print("# AUTO-GENERATED by action_table_gen.py: CONTAINS ALL CONSTRUCTED LR-SETS USED FOR PARSER CODE-GENERATION", file=f)
        dump_states(lr_sets, file=f)

    action_table[:] = encode_act_table(action_table)
    gen_cpp_file();
    # for a in action_table:
    #     print(a)
    # for r in expanded_goto_table:
    #     print(r)
    

def gen_cpp_file():

    with open("../src/parser/parsing_tables.h", "w") as f:
        f.write("//AUTO GENERATED\n")
        f.write("#pragma once\n")
        f.write("#include <vector>\n")
        f.write("#include \"../util/token.h\"\n\n")

        f.write("enum class Nonterminal { Program = 0, S, Block, AExp, BExp };\n\n")

        f.write(f"constexpr int NUM_STATES = { len(lr_sets)};\n")
        f.write(f"constexpr int NUM_TERMINALS = { len(TERMINALS)};\n")
        f.write(f"constexpr int NUM_NONTERMINALS = { len(NONTERMINALS)};\n")
        f.write(f"constexpr int NUM_PRODUCTIONS = { len(GRAMMAR)};\n\n")

        # Lock the contract: TokenType enum ordinals must equal table column indices.
        f.write("// Enusres that TokenType order corresponds to column indices.\n")
        for col, term in enumerate(TERMINALS):
            token = TERMINAL_TO_TOKEN[term]
            f.write(f"static_assert(static_cast<int>(TokenType::{token}) == {col}, \"TokenType::{token} index does not match parser column {col}\");\n")
        f.write(f"static_assert(static_cast<int>(TokenType::END_OF_FILE) + 1 == NUM_TERMINALS, \"TokenType count does not match NUM_TERMINALS\");\n\n")

        f.write("// Action encoding: 0=error, 1=shift, 2=accept, -p=reduce by production p.\n")
        f.write("inline const std::vector<std::vector<int>> ACTION_TABLE = {\n")
        for row in action_table:
            line = "{"
            for item in row:
                line += f"{item:3d}, "
            line+="},\n"
            f.write("   " + line)
        f.write("};\n\n")

        f.write("// value refers to index of next state, -1 corresponds to error state\n")
        f.write("inline const std::vector<std::vector<int>> GOTO_TABLE = {\n")
        for row in expanded_goto_table:
            line = "{"
            for item in row:
                if not item is None:
                    line += f"{item:3d}, "
                else:
                    line += f"{-1:3d}, "
            line+="},\n"
            f.write("   " + line)
        f.write("};\n\n")

        f.write("inline const std::vector<int> PRODUCTION_LENGTH = {\n")
        for _,rhs in GRAMMAR:
            f.write(f"{len(rhs)}, ")
        f.write("\n};\n\n")

        f.write("inline const std::vector<Nonterminal> PRODUCTION_LHS = {\n")
        for lhs,_ in GRAMMAR:
            f.write(f"Nonterminal::{lhs}, ")
        f.write("\n};\n\n")

if __name__ == "__main__":
    main()