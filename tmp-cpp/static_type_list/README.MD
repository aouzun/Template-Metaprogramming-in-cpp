# static-type-list

This folder contains to implementation of **type_list**, and metafunctions to support usage of it.

**type_list** is a collection of types like ``std::tuple``, however it does not store any values.

# Metafunctions

Following metafunctions of **type_list** are added.

## length

Returns the total number of types stored within a given **type_list**.

## front

Returns the type of the front (first) element of a give **type_list**. 

## back

Returns the type of the back (last) element of a given **type_list**.

## nth_element

Returns the type of the element at nth index of a given **type_list**.

## push_front

For a given type *Arg* and a **type_list**, push *Arg* at the front of the list and return the new list.

## push_back

For a given type *Arg* and a **type_list**, push *Arg* at the back of the list and return the new list.

## pop_front

Removes the front (first) element from the given **type_list** and returns the new list.

## pop_back

Removes the back (last) element from the given **type_list** and returns the new list.

## index_of

For a given type *Arg* and a **type_list** returns the index of the first found *Arg*. 

If it is not found, a negative number is returned.

## contains

For a given type *Arg* and **type_list** returns true if *Arg* is found within the **type_list**.

Otherwise returns false.

## count

For a given type *Arg* and a **type_list** returns the number of occurrences of *Arg* within the **type_list**.

## concat

For given two **type_list**, concatante them into one **type_list** and return it.

## remove

Remove the first instance of given *Arg* from the given **type_list**, if it exists.

No-op if *Arg* does not exist within **type_list**.

## remove_all

Remove all instances of given *Arg* from the given **type_list**, if it exists. No-op if *Arg* does not exist within **type_list**.

## unique

TODO: Remove all duplicated types within a given **type_list**.

## search

Searches the given **type_list** with a given UnaryPredicate meta function. Returns the first type matching the predicate. If no type is found, return false.
