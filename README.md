# Luter
A NEST.js route conflict checker

## Description
**Luter** interpret the nestjs controller file passed in as path argument and extracts all routes inserting them in trie data structure for evaluation

## Evaluation criteria
if a route contains a **":param"** followed by another param in the same route it becomes a conflict beacause other routes will be injected in as param for that endpoint
bug founded in nestjs/swagger

## Further improvments
- add the possibility to scan an entire nestjs project;
- refactor and optimization;
- auto correction of bad routes?;
- integration in nestjs codebase?;

## Usage
- Make the executable
```zsh
make
```
- Launch the exe
```zsh
./luter x.controller.ts
```
<br><br/>
> **P.S.** I chose to use C not because I wanted to shoot myself in the foot but because I wanted to learn the language with a project in mind. 😂
