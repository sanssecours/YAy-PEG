# Ideas

## General

---

Start with `indentation=-1` (`indentation` stored in state)

1. Some whitespace rule `whitespace(indentation+m)` matches
2. Action for `whitespace(indentation+m)`
   1. Save current indentation on stack

Add special `seq` rule that undoes stack modifications on failure
What about nested `seq` rules?

---

- Always match whitespace after newline
- Store indentation for each line
- Keep track of current line

---

Store old indentation in match function and restore old state on success
See also: https://github.com/taocpp/PEGTL/issues/13

## Simplified Parsing

1. Check for indentation `push_indent`
2.
   1. Same level:
       1. Add to current key
       2. Remove indentation
   1. Increased level:
       1. Add new level
       2. Remove indentation

```c++
node = seq<push_indent, sor<sibling, child>>; // Sibling: Indentation[end] == Indentation[end-1]
// If Indentation is the same, remove top element from indentation stack.
// Otherwise keep indentation
child = seq<node, pop_indent>;
```

```yaml
root          # [0]
  level1      # [0, 2]
    level2    # [0, 2, 6]
      level3  # [0, 2, 6, 8]
  one         # [0, 2]
  two         # [0, 2]
  three       # [0, 2]
```
