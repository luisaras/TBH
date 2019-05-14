
p <- function(folder){

	for (file in c("par8-5-c", "flat50-1", "flat75-1", "flat100-1")) {
		for (k in 0:5) {
			path = paste(folder, paste(file, k), sep = "/")
			title = paste(file, k, sep = ", k=")
			d <- read.table(paste(path, "txt", sep = "."))
			png(filename = paste(path, "png", sep = "."))
			hist(d$V5, xlab = "", ylab = "", main = title)
			dev.off()
		}
	}

}

p("greedy")
p("grasp")