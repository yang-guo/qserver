# qserver

## To build

You will need a 64-bit machine, a C compiler, linker, system header files, and RStudio to successfully build qserver.

* On OS X - install the XCode development tools, including the command line tools
* On Linux - most distros have a group-package for dev tools, compilers, headers etc. 
E.g. on Debian/Ubuntu: `sudo apt-get install build-essential`
* On Windows - install RTools: https://cran.r-project.org/bin/windows/Rtools

Now you should be able to build qserver using RStudio: open the project file `qserver.Rproj` and on the top menu go to Build > Build and Reload. You should see compiler & linker output in the Build tab on the right. The qserver library will be automatically installed to your R library path and loaded into your current R session.

## Example usage

```R
> library(qserver)
>
> h <- open_connection("localhost", 5001)
> 
> execute(h, "7*6")  # execute a query on handle h
[1] 42
> 
> k("7*6")  # Use 'k' as a shortcut - it will use the handle last opened with open_connection
[1] 42
> 
> k("\\l sp.q")  # load sp.q tables into kdb process
[1] 0
> 
> k("select from s")
   s  name status   city
1 s1 smith     20 london
2 s2 jones     10  paris
3 s3 blake     30  paris
4 s4 clark     20 london
5 s5 adams     30 athens
> 
> k("max", c(1,2,3,4)) # use an R-type in a function parameter
[1] 4
> 
> k("{x*y*z}", 3, 4, 5) # pass multiple parameters to function
[1] 60
>
> k("set", as.symbol("Rpi"), pi) # create a variable in kdb from R-type
[1] "Rpi"
```

Note that the variable is a one-element list in kdb as `pi` is a one-element vector in R:
```
q)Rpi
,3.141593
```
