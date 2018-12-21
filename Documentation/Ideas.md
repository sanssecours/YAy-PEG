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
See also:
  - https://github.com/taocpp/PEGTL/issues/13
  - https://github.com/taocpp/PEGTL/issues/32

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
node = seq<push_indent, node_or_pop_indent, pop_indent>;
node_or_pop_indent = sor<sibling_or_child, pop_indent>;
sibling_or_child = sor<child, sibling>;
sibling = seq<same_indent, content>;
child = seq<more_indent, content>;
content = sor<scalar, map>;
scalar = identifier;
map = seq<scalar, one<':'>, node>;
// child: indentation[end] > indentation[end-1] || indentation.size() <= 1
// sibling: indentation[end] == indentation[end-1]
```

```yaml
root:         # [0]
  level1:     # [0, 2]
    level2:   # [0, 2, 6]
      level3  # [0, 2, 6, 8]
  one:        # [0, 2]
  two:        # [0, 2]
  three:      # [0, 2]
```
