
proc add_two_ints_x(a int, b int) int
     => a + b;

proc add_two_ints(a int, b int) int
{
    return a + b;
}

proc main()
{
    var x [3]int = .[ 5, 6, 0 ];
    x[2] = add_two_ints(x[0], x[1]);
}


proc factorial(x int) int
{
    if x < 2 {
	return 1;
    }
    return x * factorial(x - 1);
}
