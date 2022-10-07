codethorn RC_1107.c --generate-reports --start-function=BAD --precision=0 --exploration-mode=topologic-sort --normalize-level=0 --rw-trace --tf-trace --vis --log-level=warn --tg1-memory-subgraphs --abstraction-mode=0  --array-abstraction-index=-1
#codethorn RC_1107.c --start-function=FOO --exploration-mode=topologic-sort --normalize-level=2 --log-level=info --tg1-memory-subgraphs --abstraction-mode=0  --array-abstraction-index=-1 --solver=5
cat out-of-bounds-locations.csv

# initializeGlobalVariables
# if error is recorded it's recorded with the label of the global variable declaration

