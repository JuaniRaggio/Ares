#include <lib.h>

s_time get_current_time() {
        return (s_time){
            .hours   = get_current_hour(),
            .minutes = get_current_minutes(),
            .seconds = get_current_seconds(),
        };
}
