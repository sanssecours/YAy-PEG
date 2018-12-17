# Ideas

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
