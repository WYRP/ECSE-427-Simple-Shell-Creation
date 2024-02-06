## Group Members
* Yongru Pan: 261001758
* Yixuan Qin: 261010963

## Optional Question 8
<p>if statementc support </p>
<br>

#### Usage
The "ifStatement" interprets and executes a custom "if" statement syntax for a shell running in Docker, based on the provided command line arguments. It validates the syntax by checking for equality or inequality operators, and the presence of "then" and "fi" keywords, and searches for an "else" keyword to determine the flow of execution. If identifiers start with $, it attempts to fetch their values from memory; then, based on the evaluation of the condition, it executes the command block before or after the "else" keyword. The function returns an error code if syntax validation fails or if the specified identifiers do not exist in memory.## Functionality


## Optional Question 9
#### Command
<p>my_cp</p>
<br>

#### Usage
<p>my_cp srcFileName destFileName</p>

* srcFileName: name of the source file (with extension) which you want to copy the content from.
* destFileName: name of the destination file (with extension) which you want to copy the content to.
<br>

#### Functionality
<p>"my_cp" implements the functionality of one of the usages of command "cp". It takes in two string arguments "srcFileName" and "destFileName" and copies the content of the source file to the destination file.</p>
<p>If destination file doesn't exist, "my_cp" creates a file with that name and copies the content to that file. Otherwise, my_cp overwrites the content of the destination file.</p>
<br>
