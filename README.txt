Contains files, source code etc. relevant to IndP18 final year project.

BUGS:

Only witnessed on windows, linux might be fine:
get_cpu_time() output is inconsistent. Sometimes the output will change with every loop in main, sometimes
it will change perhaps once during a video, then continue to return the same time.
This causes fps_cpu to be incorrect, often unbelievably large.
wall_fps appears to be unaffected. ( though won't be updated due to cpu time not changing)
for now I have disabled cpu_fps functionality.
Wall fps is really what we care about, cpu fps just gives a rough indication of processor utilisation

NOTE ON STYLE:

For all commits, please add usefull comments about the file(s)/changes, and reasons where applicable. Use tags to mark important commits and push these to the repository.
If a large number of changes have been made to a file, please think about committing changes one at a time to make reverting easier.

This readme belongs to the master branch; if you fork another branch please update that branch's readme to indicate the purpose of the branch.
Finally please let everyone know before you merge branches as they may still be active.