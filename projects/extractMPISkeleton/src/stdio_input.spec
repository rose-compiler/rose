;
; API specification for the input functions in C stdio
;
(api-spec stdio

  ( dep-types buffer other )

  ( default_deptype other )

  (
    (fread 4 (buffer 0))
    (scanf 2 (other 0 1))
  )
)
