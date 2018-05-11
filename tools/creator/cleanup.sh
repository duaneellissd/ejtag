#! /bin/bash

# Sometimes, Python IDEs get a little lost
# and can no longer properly highlight source
# or the completion data base is screwed up
# this cleans up, or resets the IDE 

for dname in `find . -type d -name __pycache__ -print`
do
   echo "Removing: ${dname}"
   if [ -d "${dname}" ]
   then
       rm -rf "${dname}"
   fi
done

for fname in `find . -type f -name '*.pyc' -print`
do
   echo "Removing: ${fname}"
   if [ -f "${fname}" ]
   then
       rm -f "${fname}"
   fi
done

if [ -d .vs ]
then
   rm -rf ./.vs
fi
