/*
 * config.h
 * Configuration file for u-search package.
 */

#ifndef CONFIG_H_
#define CONFIG_H_

// Scheduler server port.
#define SCHEDULERPORT "2050"

// Scheduler hostname for spider
#define SCHEDULERHOST "localhost"

// Client can choose any source port.
// It is better not to use SCHEDULERPORT so scheduler and spider can
// run on the same machine.
#define SPIDERPORT "2051"

/**
 * @def DATABASE_CONFIG
 * A macro to define path to the database config file.
 */
#define DATABASE_CONFIG "/etc/u-search/database.dat"

// Size of buffer which used to get smb directory entries.
#define BUF_SIZE 512

// Maximum size of vector with scan results.
#define VECTOR_SIZE 2048

// Name of directory to store headrs of files.
#define TMPDIR "/tmp/u-search"

// The size of file header to copy in TMPDIR to detect mime type of file.
#define HEADERSIZE 10

#endif  // CONFIG_H_
