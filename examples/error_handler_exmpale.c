#include <ucutils/error_handler.h>

static return_t init_driver(int ok) {
  return_fail_if(!ok, -1);
  return_value(0);
}

static return_t run(void) {
  return_on_fail(init_driver(1));
  return_on_fail(init_driver(0));
  return_success();
}

int main(void) {
  return_t success = init_driver(1);
  if (!return_is_success(success) || success.value != 0) {
    return 1;
  }

  return_t failure = init_driver(0);
  if (!return_is_failure(failure) || failure.value != -1) {
    return 2;
  }

  return_t propagated = run();
  if (!return_is_failure(propagated) || propagated.value != -1) {
    return 3;
  }

  return 0;
}
