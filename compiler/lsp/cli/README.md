# Ocen LSP CLI

The Ocen LSP server has a 2-part system. The first part is the server itself that communicates with the LSP client,
and the second part is this CLI that is responsible for all the language-specific logic. 

These are not combined together for two reasons:
1. Memory Management: Ocen does not have a garbage collector, and the current compiler does not properly clean up memory.
   This is because we have tons of circular references with no clear ownership on any of the data. It is fine for a single
   compiler run, but it is not fine for a long-running server where this leaked memory will accumulate.

2. LSP Server Libraries: Writing a from-scratch implementation of the LSP server including the JSON/HTTP parsing is tedious,
   and error prone if we mess up the schemas for some of the operations. Currently, this server is written in TypeScript
   using the libraries provided by Microsoft.
    - In the future, this might be a candidate to rewrite in Ocen, but we will still likely maintain the 2-part system.
