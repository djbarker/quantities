## Quantities

A C++ template meta-programming library for enforcing dimensional consistency at compile time. Inspired by boost units, partly written as an exercise for myself and also to handle mixed types nicely.

### Use

Physical quantities can be wrapped in a `quantity` class which enforces that calculations involve only the correct physical dimensions. For example `quanity<mass> m = 2.0;` would create a variable `m` which has dimensions of mass. If we were to multiply this by an acceleration to get a force we would only be able to store it in a variable of type `quantity<force>`

    quantity<mass> m = 2.0;
    quantity<acceleration> a = 3.0;
    quantity<force> f = m*a; // okay
    quantity<work> w = m*a; // causes compiler error

In this way accidental attempts to store variables in the wrong types are caught at compile time.

We can also save ourselves some typing through the use of the `auto` keyword. The last line in the example above would become `auto f = m*a;` and the compiler automatically deduces the correct type.
