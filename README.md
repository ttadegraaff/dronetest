# Drupal SP Memberform

New membership form for SP.

## Testing

1. First, run `composer install` in the project directory to install the testing software
2. Be sure to run Selenium in the background and install an appropiate webdriver (in our case, Chrome)
3. Set `export CIVIDEV_USER=username` and `export CIVIDEV_PASSWORD=password` to the appropiate username and password
4. Run `./vendor/bin/codecept run` to run the tests.
