from pathlib import Path

found_nums = []

with open("all_runs.txt","r") as argfile:
    #read all lines
    lines = argfile.readlines()
    to_write = []
    
    #process existing files
    for file in Path('/opt/ppd/scratch/szwarcer/paragem/output').glob("*.csv"):
        found = str(file).split('/')[-1]
        found = int(found.split('.')[0])
        found_nums.append(found)
    
    #compare with run numbers in argfile
    for line in lines:
        if not int(line.split(",")[-1]) in found_nums:
            to_write.append(line)

print(str(len(to_write)) + " missing: ")
print(to_write)

#overwrite argfile
with open("all_runs.txt","w") as argfile:
     argfile.writelines(to_write)

