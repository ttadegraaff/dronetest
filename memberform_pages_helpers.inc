<?php

/**
 * @file Helperfunctions for memberform pages.inc.
 */

/**
 * Splits name in first, middle, and last name part.
 *
 * @param string $name
 *   The name variable.
 *
 * @return array
 *   Split name parts.
 */
function memberform_split_name($name) {
  $matches = array();
  $name = trim($name);
  $name = trim($name, '.,');
  // Remove comma from name.
  $comma_pos = strpos($name, ',');
  if ($comma_pos !== FALSE) {
    $first_half = trim(substr($name, 0, $comma_pos));
    $last_half = trim(substr($name, $comma_pos + 1));
    $name = $last_half . ' ' . $first_half;
  }
  // Replace terms.
  $replace = array(
    'v' => 'van',
    'v.' => 'van',
    'vd' => 'van de',
    'vd.' => 'van de',
    'v.d.' => 'van de',
    'v/d' => 'van de',
  );
  foreach ($replace as $orig => $new) {
    if (strpos($name, ' ' . $orig . ' ') !== FALSE) {
      $name = str_replace($orig, $new, $name);
    }
  }
  // Regexes for title and middle names.
  $titles_regex = 'Mr|mr|Mevr|mevr|Dhr|dhr|Ir|ir|Ing|ing|Bc|bc|Ms|ms|Drs|drs|Dr|dr';
  $middle_names_array = memberform_middle_names();
  $middle_names_regex = implode('|', $middle_names_array);
  // Split name on middle_name.
  $regex = "@^(?'title'(\b(" . $titles_regex . ")\b\.?\s)?)(?'first_name'(([A-ZÀÈÌÒÙÁÉÍÓÚÝÂÊÎÔÛÃÑÕÄËÏÖÜŸÇÅa-zàèìòùáéíóúýâêîôûãñõäëïöü¡¿çãñõ'\-\.]+)\s*)+?)(?'middle_name'(\b(" . $middle_names_regex . ")\b\s*))(?'last_name'(\b[A-ZÀÈÌÒÙÁÉÍÓÚÝÂÊÎÔÛÃÑÕÄËÏÖÜŸÇÅa-zàèìòùáéíóúýâêîôûãñõäëïöü¡¿çãñõ'][a-zàèìòùáéíóúýâêîôûãñõäëïöü¡¿çãñõ'\-]+\s?)+)$@";
  $result = preg_match($regex, $name, $matches);
  if (!preg_match($regex, $name, $matches)) {
    // Split name on middle name, last name first.
    $regex = "@^(?'title'(\b(" . $titles_regex . ")\b\.?\s)?)(?'last_name'(\b[A-ZÀÈÌÒÙÁÉÍÓÚÝÂÊÎÔÛÃÑÕÄËÏÖÜŸÇÅa-zàèìòùáéíóúýâêîôûãñõäëïöü¡¿çãñõ'][A-ZÀÈÌÒÙÁÉÍÓÚÝÂÊÎÔÛÃÑÕÄËÏÖÜŸÇÅa-zàèìòùáéíóúýâêîôûãñõäëïöü¡¿çãñõ'\-]+\s?)+),?\s(?'first_name'(([A-ZÀÈÌÒÙÁÉÍÓÚÝÂÊÎÔÛÃÑÕÄËÏÖÜŸÇÅa-zàèìòùáéíóúýâêîôûãñõäëïöü¡¿çãñõ'\-\.]+)\s*)+?)(?'middle_name'(\b(" . $middle_names_regex . ")\b\s*))$@";
    $result = preg_match($regex, $name, $matches);
    if (!preg_match($regex, $name, $matches)) {
      // Split name without middle_name in first name and last name(s).
      $regex = "@^(?'title'(\b(" . $titles_regex . ")\b\.?\s)?)(?'first_name'(([A-ZÀÈÌÒÙÁÉÍÓÚÝÂÊÎÔÛÃÑÕÄËÏÖÜŸÇÅa-zàèìòùáéíóúýâêîôûãñõäëïöü¡¿çãñõ'\-\.]+)\s*)+?)(?'last_name'(\b[A-ZÀÈÌÒÙÁÉÍÓÚÝÂÊÎÔÛÃÑÕÄËÏÖÜŸÇÅa-zàèìòùáéíóúýâêîôûãñõäëïöü¡¿çãñõ'][A-ZÀÈÌÒÙÁÉÍÓÚÝÂÊÎÔÛÃÑÕÄËÏÖÜŸÇÅa-zàèìòùáéíóúýâêîôûãñõäëïöü¡¿çãñõ'\-]+\s?)+)$@";
      $result = preg_match($regex, $name, $matches);
      if (!preg_match($regex, $name, $matches)) {
        // Split name in first name and last name, last name first.
        $regex = "@^(?'title'(\b(" . $titles_regex . ")\b\.?\s)?)(?'last_name'(\b[A-ZÀÈÌÒÙÁÉÍÓÚÝÂÊÎÔÛÃÑÕÄËÏÖÜŸÇÅa-zàèìòùáéíóúýâêîôûãñõäëïöü¡¿çãñõ'][A-ZÀÈÌÒÙÁÉÍÓÚÝÂÊÎÔÛÃÑÕÄËÏÖÜŸÇÅa-zàèìòùáéíóúýâêîôûãñõäëïöü¡¿çãñõ'\-]+\s?)+),?\s+(?'first_name'((([A-ZÀÈÌÒÙÁÉÍÓÚÝÂÊÎÔÛÃÑÕÄËÏÖÜŸÇÅa-zàèìòùáéíóúýâêîôûãñõäëïöü¡¿çãñõ']\.)+)\s*)+?)$@";
        $result = preg_match($regex, $name, $matches);
        if (!preg_match($regex, $name, $matches)) {
          // Check if only first name.
          $regex = "@^(?'first_name'([A-ZÀÈÌÒÙÁÉÍÓÚÝÂÊÎÔÛÃÑÕÄËÏÖÜŸÇÅa-zàèìòùáéíóúýâêîôûãñõäëïöü¡¿çãñõ'][a-zàèìòùáéíóúýâêîôûãñõäëïöü¡¿çãñõ'\-]+))$@";
          $result = preg_match($regex, $name, $matches);
          if (!preg_match($regex, $name, $matches)) {
            // No match found.
            return FALSE;
          }
        }
      }
    }
  }
  // Set first character uppercase.
  $parts = array(
    'first_name',
    'last_name',
  );
  foreach ($parts as $part) {
    if (!empty($matches[$part])) {
      $matches[$part] = ucfirst($matches[$part]);
    }
  }
  return $matches;
}

/**
 * Return array with middle names.
 *
 * @return array
 *   Middle names.
 */
function memberform_middle_names() {
  return array(
    "Uijt te de", "van van de", "uijt te de", "Voor in ‘t", "De van der", "de van der", "Van van de", "voor in ‘t", "Uit te de", "onder den", "Voor in t", "onder het", "van de l’", "De die le", "de die le", "Van de l’", "voor in t", "Onder het", "Onder den", "uit te de", "over het", "uijt den", "Onder de", "boven d’", "uijt ten", "Uijt ten", "over den", "Over den", "onder de", "onder ‘t", "Onder ‘t", "Van de l", "Boven d’", "van de l", "voor den", "Over het", "Uijt den", "Voor den", "Over ‘t", "Boven d", "Von dem", "boven d", "Von der", "von den", "Von den", "von der", "von dem", "Onder t", "van der", "Van ter", "Van gen", "van het", "van gen", "Van het", "van ter", "over ‘t", "van den", "Van den", "Van Der", "Van der", "onder t", "Van Den", "bij den", "auf dem", "Auf den", "Auf dem", "voor de", "uit het", "Uit het", "auf den", "Auf der", "Voor de", "voor ‘t", "auf ter", "Auf ter", "auf der", "aan het", "Aan het", "uijt de", "vor der", "Uijt de", "uijt ‘t", "Uijt ‘t", "Vor der", "Uit ten", "Aan der", "aan der", "aan den", "Aan den", "uit ten", "uit den", "Uit den", "aus der", "Bij het", "Bij den", "Voor ‘t", "over de", "Over de", "bij het", "Aus dem", "aus dem", "Aus den", "Aus der", "aus den", "in het", "die le", "In der", "in der", "In het", "Uit de", "van ‘t", "Van de", "van De", "van de", "Van ‘t", "Uit ‘t", "In den", "uit de", "uit ‘t", "in den", "Op ten", "op het", "Over t", "Op het", "Die le", "op gen", "over t", "op ten", "Von ‘t", "Bij ‘t", "aus ‘m", "bij ‘t", "Bij de", "von ‘t", "bij de", "Op gen", "op der", "Aan ‘t", "aan ‘t", "De las", "de las", "Van la", "van la", "Aan de", "aan de", "op den", "Op der", "De die", "de die", "Op den", "Aus ‘m", "Onder", "op ‘t", "Op ‘t", "Op de", "op de", "onder", "Uit t", "Van t", "De l’", "uit t", "Dalla", "de l’", "De la", "de le", "De le", "de la", "Von t", "bij t", "Am de", "aan t", "Aan t", "am de", "Aus m", "Bij t", "von t", "aus m", "Degli", "dalla", "van t", "In de", "in ‘t", "In ‘t", "della", "in de", "unter", "Unter", "Della", "degli", "deca", "uijt", "Over", "over", "Voor", "in t", "In t", "Uijt", "voor", "op t", "De l", "thoe", "de l", "thor", "Deca", "Thor", "Dal’", "Thoe", "Op t", "dal’", "zum", "Zur", "toe", "zur", "tot", "Toe", "Tot", "uit", "vom", "Von", "Vom", "ver", "Ver", "van", "Van", "vor", "Vor", "Uit", "von", "Zum", "Los", "Den", "den", "del", "Del", "Dei", "dei", "Der", "der", "dos", "Het", "Dos", "don", "Des", "des", "das", "Das", "Aus", "aus", "auf", "Auf", "Aan", "aan", "Ben", "ben", "Dal", "dal", "bin", "Bin", "Bij", "bij", "het", "Don", "Ter", "las", "ter", "ten", "Ten", "los", "les", "Les", "Tho", "Las", "tho", "d’", "‘T", "‘t", "D’", "Da", "da", "l’", "op", "S’", "‘s", "s’", "te", "af", "am", "Am", "al", "Af", "de", "Al", "zu", "Zu", "to", "Te", "De", "du", "El", "Du", "Le", "do", "Op", "el", "la", "in", "L’", "In", "im", "La", "Im", "Do", "To", "lo", "di", "‘S", "Of", "Lo", "le", "of", "Di", "A", "l", "S", "s", "L", "I", "T", "t", "D", "d", "i", "a",
  );
}

/**
 * Doe een postcode-lookup op basis van postcode en huisnummer.
 *
 * API-method van Jaap voor Pro6pp
 *
 * @param string $postcode
 *   Nederlandse postcode.
 * @param string $huisnummer
 *   Huisnummer.
 *
 * @return object
 *   Blah.
 */
function memberform_postcode_lookup($postcode, $huisnummer = '') {

  $api = spciviapi_get_api();
  $api->PostcodeNL->get([
    'postcode'   => $postcode,
    'huisnummer' => $huisnummer,
  ]);
  $res = $api->result();

  if ($res && !$res->is_error) {
    if (count($res->values) > 0) {
      return array_shift($res->values);
    }
    else {
      return 'Deze combinatie van postcode en huisnummer kon niet worden gevonden! Weet je zeker dat deze correct is?';
    }
  }
  watchdog('memberform', 'Er is een fout opgetreden bij het ophalen van postcode gegeven via de civiapi.', array(), WATCHDOG_ERROR);
}


/**
 * Iban check.
 *
 *  From:
 *  http://monshouwer.org/code-snipets/check-iban-bank-account-number-in-php/
 *
 * @param string $iban
 *   Iban number to check.
 *
 * @return bool
 *   True if is valid, else false.
 */
function memberform_check_iban($iban) {
  // Normalize input (remove spaces and make upcase).
  $iban = strtoupper(str_replace(' ', '', $iban));

  if (preg_match('/^[A-Z]{2}[0-9]{2}[A-Z0-9]{1,30}$/', $iban)) {
    $country = substr($iban, 0, 2);
    $check = intval(substr($iban, 2, 2));
    $account = substr($iban, 4);

    // To numeric representation.
    $search = range('A', 'Z');
    foreach (range(10, 35) as $tmp) {
      $replace[] = strval($tmp);
    }
    $numstr = str_replace($search, $replace, $account . $country . '00');

    // Calculate checksum.
    $checksum = intval(substr($numstr, 0, 1));
    for ($pos = 1; $pos < strlen($numstr); $pos++) {
      $checksum *= 10;
      $checksum += intval(substr($numstr, $pos, 1));
      $checksum %= 97;
    }

    return ((98 - $checksum) == $check);
  }
  else {
    return FALSE;
  }
}
