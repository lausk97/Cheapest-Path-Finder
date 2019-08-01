# Cheapest-Path-Finder
A project of University of Melbourne COMP10002 (Foundations of Algorithms) 2017 semester 2

## About the code
It uses Dijkstra's algorithm to find a path from a point to another in the form of a grid

## Test Files Format
The first line of the test file is the number of rows and columns

e.g. 7 5

the example above means 7 rows and 5 columns in the grid map


For each of the following lines, the first element in that particular line is the current position and the rest of the elements are the cost to each direction from the current position.

e.g. 6b 999 12 18 22

the example above means 6b to 7b costs 999, 6b to 6a costs 12, 6b to 5b costs 18 and 6b to 6c costs 22.

A path that takes 999 means it cannot go through that direction.
