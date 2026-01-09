#ifndef TIMER_H
#define TIMER_H

#include <time.h>
#include <unistd.h>

// Récupère le timestamp actuel (secondes depuis epoch)
long get_current_timestamp();

// Attend N secondes
void wait_seconds(int seconds);

// Attend jusqu'à un timestamp précis
void wait_until(long target_timestamp);

#endif
