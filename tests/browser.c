#include <stdio.h>
#include <assert.h>
#include "../src/browser.h"

int main(void) {

    assert(browserOpen("8080"));

    return 0;
}
