# City Mapper

## Introduction
This repository contains the code for the City Mapper project.
City Mapper is a project that aims to implement the djikstra algorithm using C++ STL structures to solve the problem of finding the shortest path between two stations in a city.
To do so, we have to model the city as a graph, where each node is a station and each edge is a line.

## Data
This project uses two files:
* `s.csv`: contains the locations of the stations, their names and the lines they are on.
* `c.csv`: contains the transfer time between two stations.

# Usage 
The program is implemented in the `main` function.
It takes as arguments: 
* `s.csv`: the path to the file containing the stations.
* `c.csv`: the path to the file containing the transfers.

First, use the makefile that is provided to compile the program.

Once the program is compiled, you can run it with the following command:
```bash
./main ../data/s.csv ../data/c.csv
```

You will see the following output:
```bash
Bienvenue dans RATP mapper !

<Nouvelle recherche>
[1]Recherche d'itineraire par ID
[2]Quitter le programme
Saisir votre choix (1, 2 ) et appuyer sur <ENTREE>:
1
```

## Improvements
Several improvements can be made to the program, including:
* Adding a menu to choose between different algorithms.
* Adding a menu to choose between different data files.
* Adding a menu to choose between different data formats.
* Adding a menu to choose between different data structures.
* Adding an GUI.








  