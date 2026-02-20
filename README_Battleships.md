# **README - Battleships game**

## **Battleships**

The problem involves reading a value corresponding to the number of Battleships matches that will be played by the two players, and then, for each of them, reading the board dimensions and determining the maximum number of ships that can be placed. I allocate arrays to store both the game boards (`board`), information about the placed ships (`fleet`), and game statistics for the extension (`game_stats`).

From `main`, I call the `place_ships` function, which initializes ship placement as follows:

-	reads the ship type, orientation, and starting coordinates;
-	 checks whether the placement is valid (through the `check_valid_placement` function), and if so, determines the ship length (via `ship_length`) and its ending coordinates (via `ship_span`), then places it on the corresponding player's board and adds the ship information to the player's fleet array (players alternate placements, achieved by switching the `player` variable at each iteration);

  * the `check_valid_placement` function follows this algorithm: it verifies whether a ship placement is invalid in one of the following three cases:
    1. it overlaps with another ship (case verified by `check_overlapping`);
    2. it goes outside the board
    3. the given coordinates are not found on the board (i.e., they exceed the read dimensions) — cases 2 and 3 are verified by `check_out_of_bounds`.
  * if the ship placement falls into one of these cases, a corresponding message is displayed, customized with the ship’s coordinates and orientation, using the `print_invalid_placing_message` function.
* the function also counts the board cells covered by ships, as well as the number of ships placed, in order to later compute the accuracies in the statistics.

  * the values of these variables are loaded into the `game_stats[].player_no[(1/2)]` vector depending on the player, back in the `main` function, after all ships have been placed.
* after placements, the players’ boards are displayed with all ships, and then the attack phase begins.

The `attacks` function centralizes the entire attack process, which is also performed alternately.

* attacks are read until at least one of the players has destroyed all of the opponent’s ships;

* attacks are simulated through the `execute_attacks` function, which has a double role:

  * on one hand, it changes the board configuration if:

    * a cell containing part of a ship is hit (and sets its value to -1 so it is later considered an already attacked coordinate pair);
    * a cell containing the starting point of a ship (thus destroying the ship) is hit and returns the value 1 so destroyed ships can be counted back in `attacks` in the two variables `destroyed_ships_p1/2`; in any other case (simple hit, repeated hit, miss), it returns 0;
    * corresponding messages are then displayed through the `print_hitting_message` and `print_destroying_message` functions;
  * on the other hand, it displays messages if the attacked coordinates were already hit or if the attack missed;
  * additionally, in this function, the number of attacks executed by a player and how many of them were successful are counted, also for the extension statistics.

* at the end, the function displays which of the two players won the match, and back in `main`, the entire process is repeated until all games are completed.

## **Game Extension – Statistics and Hill-Cipher Encryption/Decryption Algorithm**

After all games are completed, the `game_extension` function is called, which processes the statistics section using the previously filled `game_stats` vector. For each game and each player, it stores the number of ships, total number of attacks, number of successful attacks, and number of board cells occupied by ships, in order to compute the requested accuracies per game and across all matches.

* The first step is creating the encryption alphabet (`create_alphabet`), and then reading the encryption/decryption key;

  * however, if the key is not specified (`-`), meaning unencrypted output is desired and the requested statistic is also given unencrypted, the memory allocated for the key and alphabet is freed.
* then, the string(s) representing the requested statistics are read, and depending on the key, the following cases are handled:

  1. *key not specified* (`-`) → the statistics text is processed directly and displayed at the end
  2. *key specified* → the statistics text is first decrypted, then processed, and finally re-encrypted and displayed

### *1. Key Not Specified*

The `statistics` function is called directly with the statistics text (`initial_text`), which is further processed:

* * here I used the character positions as they appear, not as stored in memory; in the code indexing starts from character 0;

- the player is extracted (O – 1 or T – 2) based on the first character;
- the number of desired games is extracted from the second character (T – all, U – a single one);
- the game index referred to (characters 3–5) is extracted using `determine_number_from_string`, which forms the integer based on the number provided in the string;

  * character-to-digit conversion is performed and zero-padding is removed;
- the required accuracy percentage (characters 6–11) is extracted by splitting it into integer and fractional parts (also using `determine_number_from_string`) and the final double accuracy is computed by dividing by 100.0, preserving two decimal places;
- the requested type (MIN/MAX) (characters 12–14) is extracted, and based on this information the four possible statistic calculations are determined:

#### 1. **Option 1** (all games T + type MIN) – `option_1`

* computes the sum of all placed ships (denominator of the total weighted average accuracy formula), computes the per-match accuracy as the ratio between successful hits and total hits, then multiplies by the number of ships to obtain the numerator;
* determines the gain in total accuracy if that match’s accuracy were changed from its current value to 100%;
* computes the total accuracy (`current_acc`) and compares it to the target (`target_acc`), then sorts potential gains in descending order to obtain the minimum number of matches — essentially modifying as few matches as possible, starting with those that provide the largest gain (lowest accuracies);
* determines how many matches are required and stops once the target is exceeded.

#### 2. **Option 2** (single game U + type MIN) – `option_2`

* computes the match accuracy as the ratio between successful hits and total hits, then modifies a copy of it by increasing successful hits until the target accuracy is reached.

#### 3. **Option 3** (all games T + type MAX) – `option_3`

* computes the sum of all placed ships, calculates per-match accuracy, multiplies by the number of ships to obtain the numerator;
* determines the loss in total accuracy if that match’s accuracy were changed from its current value to 0%;
* computes `current_acc`, compares it with `target_acc`, and sorts potential losses in ascending order to obtain the maximum number of matches — modifying as many matches as possible while prioritizing those with the smallest impact;
* determines how many matches are required and stops once the value drops below the target.

#### 4. **Option 4** (single game U + type MAX) – `option_4`

* computes per-match accuracy, then repeatedly decreases it by removing one unsuccessful hit at a time, until the target accuracy is reached.

After processing, each function calls `create_final_text`, where the requested accuracy (per match/total) and the determined number (hits/matches) are concatenated in the required format;

* the given accuracy is truncated (double with 6 decimals → only 2 kept) using `truncate`, zero-padding is applied where necessary, and digit-to-character conversion is performed for all elements of the final string.

### *2. Key Specified*

If a key is specified, the Hill-Cipher encryption/decryption algorithm is used:

* **Decrypting the statistics text:**

  * compute the square root of the key length and create its matrix according to the alphabet encoding;
  * since the statistics text length is not equal to the square root of the key length, decryption is performed in blocks of that size using `decrypt_long_message`; remaining characters (a block of size n) are decrypted using the first n*n values of the key matrix (selected row-wise);
  * each decrypted block is concatenated into `final_text`;
  * decryption follows the algorithm via `decrypt_message`:

    1. create the key matrix and the column matrix of the text block (`create_key_matrix`, `create_message_matrix`);
    2. compute the determinant recursively using first-row expansion (`calculate_matrix_determinant`) and compute the multiplicative inverse (`calculate_multiplicative_inverse`);
    3. compute the adjugate matrix (`calculate_adjugate_matrix`) by calculating cofactors using minors (`calculate_minor`);
    4. apply modulo 37 to the inverse and generate the text based on multiplying the inverse with the message matrix (`multiply_matrix` and `create_final_text`).

* the resulting decrypted text overwrites `initial_text`;

* the statistics processing described in case *1. Key Not Specified* is applied;

* the final processed text overwrites `initial_text` again and is considered the text to be encrypted;

* **Encryption** follows the same block conditions and steps:

  1. create the key matrix and message column matrix;
  2. multiply the two matrices (`multiply_matrix`), obtain the final matrix, transform it into text, and concatenate it into the final displayed text.

Reading statistics texts continues until the character `Q` is encountered, at which point the execution of `game_extension` ends and, in `main`, the final memory deallocations are performed and the program terminates.
