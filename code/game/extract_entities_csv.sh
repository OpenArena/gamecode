echo "item,quaked,description,model" > entries.csv
ls *.c | xargs -n1 python3 extract_entities.py  >> entries.csv
