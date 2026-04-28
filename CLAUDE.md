# Diode

## Worktrees

Place new git worktrees at `../diode-worktrees/<name>/`, **not**
`.claude/worktrees/<name>/`. The latter is gitignored, so the IDE doesn't
surface deltas inside it during review.

```bash
git worktree add ../diode-worktrees/<name> -b <name>
```
