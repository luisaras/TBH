
for (file in c("greedy/par8-5-c", "greedy/flat50-1", "greedy/flat75-1", "greedy/flat100-1")) {
	for (k in 0:5) {
		name = paste(file, k, sep = " ")
		d <- read.table(paste(name, "txt", sep = "."))
		png(filename = paste(name, "png", sep = "."))
		hist(d$V5)
		dev.off()
	}
}

for (file in c("grasp/par8-5-c", "grasp/flat50-1", "grasp/flat75-1", "grasp/flat100-1")) {
	for (k in 0:5) {
		name = paste(file, k, sep = " ")
		d <- read.table(paste(name, "txt", sep = "."))
		png(filename = paste(name, "png", sep = "."))
		hist(d$V5)
		dev.off()
	}
}