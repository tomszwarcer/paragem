counter = 1
with open("all_runs.txt", "w") as argfile:
    for i in [30.0,40.0,50.0,60.0]:
        for j in range(10):
            argfile.write(str(i) + "," + str(counter) + "\n")
            counter += 1

    

