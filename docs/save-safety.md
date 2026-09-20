# Save Safety

Deadly Premonition uses one live save file:

```text
savedata\dp.sav
```

The vanilla game opens that file destructively when saving. ZachFix can wrap the operation in a transactional path so an incomplete or rejected new save does not replace the previous live file.

## Configuration

```ini
[SaveSafety]
Enabled = true
BackupCount = 10
```

`BackupCount` accepts `1..100`.

Save Safety is initialized from the INI at startup.

## Transactional write

With Save Safety enabled, the destructive save is redirected to:

```text
dp.sav.zachtmp
```

The game writes its normal save bytes to the temporary file. ZachFix then:

1. flushes the temporary file;
2. verifies its final size;
3. reads it back;
4. runs conservative structural validation;
5. creates and verifies a compressed backup of the previous live save;
6. replaces the live `dp.sav` with a write-through move.

If a required stage fails before replacement, the previous live save is preserved.

ZachFix does not rewrite gameplay fields inside successful saves.

## Rolling backups

Backups are standard ZIP archives under:

```text
ZachFix\save_backups\dp.sav\
```

A normal backup is named like:

```text
dp_<timestamp>.zip
```

Each archive contains a restorable `dp.sav`. Archives are verified after creation before the live save may be replaced.

The oldest backups are pruned after successful backup creation according to `BackupCount`.

## Rejected-save bundles

If a transactional save is rejected, ZachFix attempts to create:

```text
ZachFix\save_backups\dp.sav\failed\failure_<timestamp>.zip
```

The bundle can contain:

- `before.sav` - previous live save, when present;
- `failed.sav` - rejected transactional save;
- `ZachFix.log`;
- `reason.txt`.

If archiving itself fails, ZachFix retains the raw evidence rather than deleting the failed data.
