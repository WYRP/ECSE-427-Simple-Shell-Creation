## Yongru Pan, 261001758 Yixuan Qin, 261010963

## Option:
No option was implemented

## Structural Clarification
page struct: 
    index[]: an int array which stores location of its content in the frame stores
    valid_bits: which of its lines are valid (not empty)
    page number (page_index): a program may have more than three lines of code, page_index indicates which page of the program it is.
    page_pid: indicates to which process this page belongs to
