# Xbox color / output restoration

## Production status

ZachFix production contains an optional Xbox-oriented scene grading/output path. This is separate from unresolved native renderer regressions such as water and trees.

The important architectural separation is:

```text
scene-authored color grading
    ≠
water material/shader regression
    ≠
packed depth/PostFX precision
```

Production documentation distinguishes:

- Director's Cut PC tone/color path;
- restored Xbox 360 scene-authored ENV Contrast/Pitch/Chroma/Addsub grading tail;
- display/output gamma/HDTV emulation choices.

Treat this as ZachFix restoration/PostFX infrastructure, not proof that every Xbox-vs-PC material shader difference is solved.

## Status

```text
Xbox-oriented ENV grading restoration available   CONFIRMED production
all Xbox-vs-PC scene appearance differences solved DISPROVEN / too broad
```
