
#!/usr/bin/env Rscript

# Number alg1 greater than alg2
compare <- function(alg1, alg2){
	file1 <- paste("results_", ".txt", sep = alg1)
	file2 <- paste("results_", ".txt", sep = alg2)
	header <- paste(alg1, alg2, sep = " greater than ")
	a <- read.table(file1)
	b <- read.table(file2)

	nodes_success <- sum(a$V3 > b$V3)
	p_nodes <- binom.test(nodes_success, 40, alternative="greater")$p.value
	result_nodes <- paste("Nodes of ", ":", sep = header)
	print(result_nodes)
	print(paste(nodes_success, p_nodes, sep = ", p-value = "))

	time_success <- sum(a$V5 > b$V5)
	p_time <- binom.test(time_success, 40, alternative="greater")$p.value
	result_time <- paste("Seach time of ", ":", sep = header)
	print(result_time)
	print(paste(time_success, p_time, sep = ", p-value = "))

}

compare("double", "double+")
compare("default", "double")
compare("initial", "default")
compare("blind", "initial")