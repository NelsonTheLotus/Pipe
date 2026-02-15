#pragma once
// A worker is a unit of execution.
// It pulls commands from lanes and decides what gets run how.
// It then issues those commands to shell, waiting for the output.