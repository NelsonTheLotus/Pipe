#pragma once
// Shell is the executor. It takes a command,
// issues it through pipes, waits for the output
// and returns the result back to it's linked worker.