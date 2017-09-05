<?php

/**
 * @file Helperfunctions for memberform module.
 */

/**
 * Removes error messages, and stores errors in form state.
 */
function memberform_remove_errors(&$form_state) {
  // Remove form error messages.
  if (!empty($form_state['form_errors'])) {
    $errors = $form_state['form_errors'];
    $error_messages = &$_SESSION['messages']['error'];
    if (!empty($error_messages)) {
      foreach ($errors as $error) {
        if (($key = array_search($error, $error_messages)) !== FALSE) {
          unset($error_messages[$key]);
          if (empty($error_messages)) {
            unset($_SESSION['messages']['error']);
          }
        }
      }
    }
    // Store errors for display in form elements.
    $form_state['form_errors'] = $errors;
    // Get the array of errors and reset them so form will be submitted.
    $errors = &drupal_static("form_set_error", array());
    // Make sure that each error should be applied to the field.
    foreach (array_keys($errors) as $field) {
      unset($errors[$field]);
    }
  }
}

/**
 * Gets stored form data from database.
 *
 * @param int $sid
 *   Form submission id.
 *
 * @return array $storage
 *   Stored form data.
 */
function memberform_get_storage($sid = NULL, $form_name = '') {
  $storage = array();
  if (!isset($sid) && isset($_SESSION['memberform'][$form_name]['sid'])) {
    $sid = $_SESSION['memberform'][$form_name]['sid'];
  }

  if (isset($sid)) {
    // Get storage from database if available.
    $query = "SELECT form_values FROM {memberform_storage} WHERE sid = :sid";
    $result = db_query($query, array(':sid' => $sid))->fetchAssoc();
    $storage = unserialize($result['form_values']);
    $storage['sid'] = $sid;
  }
  return $storage;
}

/**
 * Stores database storage variable in database.
 *
 * @param array $storage
 *   Database storage variable.
 */
function memberform_store_form_data(array &$storage = NULL) {
  if (isset($storage)) {
    if (isset($storage['sid'])) {
      // Update database storage.
      db_update('memberform_storage')
        ->fields(array(
          'form_values' => serialize($storage),
        ))
        ->condition('sid', $storage['sid'])
        ->execute();
    }
    else {
      // Insert new database storage.
      $storage['sid'] = db_insert('memberform_storage')
        ->fields(array(
          'hash' => md5(drupal_random_bytes(32)),
          'form_values' => serialize($storage),
        ))
        ->execute();
    }
  }
}

/**
 * Return the names of the different forms defined in /etc/pages.
 *
 * @return array
 *   List of pagenames.
 */
function memberform_get_formnames() {
  $paths = glob(drupal_get_path('module', 'memberform') . '/multipage_forms/*');
  foreach ($paths as $path) {
    $result = preg_match('/^.*\/memberform_([^.]*)_pages.inc$/', $path, $matches);
    if (!empty($matches[1])) {
      $form_names[] = $matches[1];
    }
  }
  return $form_names;
}

/**
 * Submit pending submissions.
 */
function memberform_submit_pending() {

  $pending = db_select('memberform_submissions', 's')
    ->fields('s')
    ->isNotNULL('verified')
    ->isNULL('submitted')
    ->execute();

  if ($pending) {
    while ($record = $pending->fetchAssoc()) {

      try {
        $contact_id = memberform_add_membership($record);
        $error = NULL;
      }
      catch (\Exception $e) {
        $error = $e->getMessage();
        $contact_id = NULL;
      }

      db_update('sp_aanmeldingen')
        ->fields([
          'submitted'  => time(),
          'errormsg'   => $error,
          'contact_id' => $contact_id,
        ])
        ->condition('id', $record['id'])
        ->execute();
    }
  }
  return TRUE;
}

/**
 * Discription missing.
 */
function memberform_friendly_field_data($key, $value, $activity = FALSE) {
  switch ($key) {
    case 'hash':
      return [NULL, NULL];

    case 'errormsg':
      if ($value != '') {
        return ['Error', '<span style="color:#f00;font-weight:bold;">' . $value . '</span>'];
      }
      return [NULL, NULL];

    case 'id':
      return ['Website-ID', $value];

    case 'membership_type':
      return ['Lidmaatschapstype', memberform_membership_description($value, TRUE)];

    case 'initials':
      return ['Voorletters', $value];

    case 'firstname':
      return ['Voornaam', $value];

    case 'middlename':
      return ['Tussenvoegsel', $value];

    case 'lastname':
      return ['Achternaam', $value];

    case 'gender':
      return ['Geslacht', strtoupper($value)];

    case 'birthdate':
      return ['Geboortedatum', $value];

    case 'email':
      return ['E-mail', $value];

    case 'phone':
      return ['Telefoon', $value];

    case 'phone_mobile':
      return ['Mobiel', $value];

    case 'country':
      return ['Land', $value];

    case 'zipcode':
      return ['Postcode', $value];

    case 'housenumber':
      return ['Huisnummer', $value];

    case 'housenumber_suffix':
      return ['Huisnummertoevoeging', $value];

    case 'street':
      return ['Straat', $value];

    case 'city':
      return ['Plaats', $value];

    case 'contribution':
      return ['Bijdrage', number_format($value, 2, ',', '.')];

    case 'bank_account':
      return ['IBAN', $value];

    case 'payment_method':
      return ['Betaalwijze', ucfirst($value)];

    case 'welcome_present_sp':
      return ['Welkomstcadeau SP', memberform_get_gift_internal_name($value)];

    case 'welcome_present_rood':
      return ['Welkomstcadeau ROOD', memberform_get_gift_internal_name($value)];

    case 'source':
      return ['Bron', $value];

    case 'created':
      return ['Aangemeld', $value ? date('d-m-Y H:i:s', $value) : '-'];

    case 'flags':
      if ($value) {
        return ['Type', $value == 'INTERN' ? '<em>Interne aanmelding</em>' : ''];
      }
      return [NULL, NULL];

    case 'verified':
      return ['Bevestigd', $value ? date('d-m-Y H:i:s', $value) : '-'];

    case 'reminded':
      return ['Herinnerd', $value ? date('d-m-Y H:i:s', $value) : '-'];

    case 'submitted':
      return ['Toegevoegd', $value ? date('d-m-Y H:i:s', $value) : '-'];

    case 'contact_id':
      if ($value > 0 && !$activity) {
        return ['CiviCRM-ID', $value . '&nbsp;&nbsp;&nbsp; <a href="' . variable_get('spciviapi_civicrm_server') . '/civicrm/contact/view?reset=1&cid=' . $value . '">Bekijk contact in CiviCRM</a>'];
      }
      return [NULL, NULL];
  }
}

/**
 * Missing description.
 */
function memberform_add_membership($data = []) {

  // Gender-waardes: magic numbers, maar deze veranderen *echt* vrijwel nooit.
  switch ($data['gender']) {
    case 'v':
      $gender = 1;
      break;

    case 'm':
      $gender = 2;
      break;

    case 'a':
      $gender = 3;
      break;

    default:
      $gender = NULL;
      break;
  }

  // Create contact.
  $contact_data = [
    'contact_type' => 'Individual',
    'first_name'   => $data['firstname'],
    'middle_name'  => $data['middlename'],
    'last_name'    => $data['lastname'],
    'display_name' => ($data['firstname'] ? $data['firstname'] : $data['initials']) . '+' . ($data['middlename'] ? $data['middlename'] . '+' : '') . $data['lastname'],
    'birth_date'   => $data['birthdate'],
    'gender_id'    => $gender,
  ];

  if ($data['initials']) {
    $sp_initials_field = variable_get('spaanmelden_id_custom_initials');
    $contact_data[$sp_initials_field] = $data['initials'];
  }

  $contact_source_field = variable_get('spaanmelden_id_custom_source');
  if ($data['source']) {
    $contact_data[$contact_source_field] = $data['source'];
  }
  else {
    $contact_data[$contact_source_field] = _spaanmelden_get_source_website();
  }

  $api = spciviapi_get_api();
  $api->Contact->create($contact_data);
  $res = $api->result();
  if (!$res || $res->is_error || !$res->id) {
    throw new \Exception('CiviCRM API error (Contact): ' . $res->error_message);
  }

  // Get contact ID and location type ID.
  $contact_id = $res->id;
  $location_type_id = _spaanmelden_get_location_type_id();

  // Get country ID.
  $country_id = array_search($data['country'], _spaanmelden_get_country_ids());

  // Add home address.
  $api->Address->create([
    'contact_id'           => $contact_id,
    'is_primary'           => 1,
    'location_type_id'     => $location_type_id,
    // 'street_address'       => $data['street'] . ' ' . $data['housenumber'] . $data['housenumber_suffix'],
    // 'street_parsing'       => 1,
    'street_name'          => $data['street'],
    'street_number'        => $data['housenumber'],
    'street_unit'          => $data['housenumber_suffix'],
    'city'                 => $data['city'],
    'postal_code'          => $data['zipcode'],
    'country_id'           => $country_id,
  ]);
  $res = $api->result();
  if (!$res || $res->is_error) {
    throw new \Exception('CiviCRM API error (Address): ' . $res->error_message);
  }

  // Add email address.
  if ($data['email']) {
    $api->Email->create([
      'contact_id'       => $contact_id,
      'email'            => $data['email'],
      'location_type_id' => $location_type_id,
    ]);
    $res = $api->result();
    if (!$res || $res->is_error) {
      throw new \Exception('CiviCRM API error (Email): ' . $res->error_message);
    }
  }

  // Add phone number(s)
  if ($data['phone']) {

    $api->Phone->create([
      'contact_id'       => $contact_id,
      'phone'            => $data['phone'],
      'location_type_id' => $location_type_id,
      'phone_type_id'    => 1,
    ]);
    $res = $api->result();
    if (!$res || $res->is_error) {
      throw new \Exception('CiviCRM API error (Phone): ' . $res->error_message);
    }
  }
  if ($data['phone_mobile']) {

    $api->Phone->create([
      'contact_id'       => $contact_id,
      'phone'            => $data['phone_mobile'],
      'location_type_id' => $location_type_id,
      'phone_type_id'    => 2,
    ]);
    $res = $api->result();
    if (!$res || $res->is_error) {
      throw new \Exception('CiviCRM API error (Mobile): ' . $res->error_message);
    }
  }

  // Add membership.
  switch ($data['membership_type']) {

    case 'lid_sp':
      $membership_type_id = variable_get('spaanmelden_id_lid_sp');
      $financial_type_id = variable_get('spaanmelden_id_fintype_sp');
      $contribution = (float) $data['contribution'];
      if ($data['payment_method'] != 'incasso') {
        $contribution += 1;
      }
      break;

    case 'lid_sprood':
      $membership_type_id = variable_get('spaanmelden_id_lid_sprood');
      $financial_type_id = variable_get('spaanmelden_id_fintype_sprood');
      $contribution = (float) $data['contribution'];
      if ($data['payment_method'] != 'incasso') {
        $contribution += 1;
      }
      break;

    case 'lid_rood':
      $membership_type_id = variable_get('spaanmelden_id_lid_rood');
      $financial_type_id = variable_get('spaanmelden_id_fintype_rood');
      $contribution = round((float) $data['contribution'] / 4, 2);
      break;

    case 'tribune':
      $membership_type_id = variable_get('spaanmelden_id_abo_tribune');
      $financial_type_id = variable_get('spaanmelden_id_fintype_tribune');
      $contribution = ($data['payment_method'] == 'incasso') ? 5 : 6;
      break;

    case 'spanning':
      $membership_type_id = variable_get('spaanmelden_id_abo_spanning');
      $financial_type_id = variable_get('spaanmelden_id_fintype_spanning');
      $contribution = 6;
      break;

    default:
      throw new \Exception('CiviCRM API error: invalid membership type: ' . $data['membership_type'] . '.');
  }

  if ($data['payment_method'] == 'incasso') {
    $payment_instrument = variable_get('spaanmelden_id_payment_incasso');
  }
  else {
    $payment_instrument = variable_get('spaanmelden_id_payment_accept');
  }

  $membership_data = [
    'contact_id'             => $contact_id,
    'membership_type_id'     => $membership_type_id,
    'membership_start_date'  => date('Ymdhis', $data['verified']),
    'join_date'              => date('Ymdhis', $data['verified']),
    'is_override'            => 1,
    'status_id'              => variable_get('spaanmelden_id_status'),
    'total_amount'           => number_format($contribution, 2, '.', ''),
    'financial_type_id'      => $financial_type_id,
    'contribution_status_id' => 2,
    'payment_instrument_id'  => $payment_instrument,
    'new_mandaat'            => 0,
    'iban'                   => $data['bank_account'],
  ];
  if ($data['payment_method'] == 'incasso' && $data['bank_account']) {
    $membership_data['new_mandaat'] = 1;
    $membership_data['mandaat_status'] = 'FRST';
    $membership_data['mandaat_datum'] = date('Ymdhis', $data['verified']);
    $membership_data['mandaat_plaats'] = ($data['city'] ? $data['city'] : '[Onbekend]');
  }

  if (in_array($data['membership_type'], ['lid_sp', 'lid_sprood'])) {
    $sp_gift_field = variable_get('spaanmelden_id_custom_gift_sp');
    $membership_data[$sp_gift_field] = memberform_get_gift_internal_name($data['welcome_present_sp']);

    $sp_gift_field_received = variable_get('spaanmelden_id_custom_gift_sp_received');
    $membership_data[$sp_gift_field_received] = 0;
  }
  if (in_array($data['membership_type'], ['lid_sprood', 'lid_rood'])) {
    $rood_gift_field = variable_get('spaanmelden_id_custom_gift_rood');
    $membership_data[$rood_gift_field] = memberform_get_gift_internal_name($data['welcome_present_rood']);

    $rood_gift_field_received = variable_get('spaanmelden_id_custom_gift_rood_received');
    $membership_data[$rood_gift_field_received] = 0;
  }

  $membership_source_field = variable_get('spaanmelden_id_custom_source_membership');
  if ($data['source']) {
    $membership_data[$membership_source_field] = $data['source'];
  }
  else {
    $membership_data[$membership_source_field] = _spaanmelden_get_source_website();
  }

  $gezinslid_field = variable_get('spaanmelden_id_custom_gezinslid');
  $membership_data[$gezinslid_field] = 0;
  $welkomstbrief_field = variable_get('spaanmelden_id_custom_welkomstbrief');
  $membership_data[$welkomstbrief_field] = 0;

  $api->Membership->spcreate($membership_data);
  $res = $api->result();

  if (!$res || $res->is_error) {
    throw new \Exception('CiviCRM API error (Membership): ' . $res->error_message . ' ' . print_r($membership_data, TRUE));
  }

  // Add activity record.
  $data['submitted'] = time();
  $activity_data = '';
  foreach ($data as $key => $value) {
    list($key, $value) = _spaanmelden_friendly_field_data($key, $value, TRUE);
    if ($key) {
      $activity_data .= $key . ': ' . $value . '<br />';
    }
  }

  $api->Activity->Create([
    'source_contact_id'  => variable_get('spaanmelden_id_activity_contact'),
    'target_contact_id'  => $contact_id,
    'activity_name'      => variable_get('spaanmelden_id_activity'),
    'activity_subject'   => 'Aanmelding als ' . memberform_membership_description($data['membership_type']),
    'activity_date_time' => date('YmdHis', $data['verified']),
    'details'            => 'De volgende aanmelding is via de website ontvangen op ' . date('d-m-Y H:i:s', $data['created']) . ' en bevestigd op ' . date('d-m-Y H:i:s', $data['verified']) . ':<br /><br />' . $activity_data,
  ]);
  $res = $api->result();
  if (!$res || $res->is_error) {
    throw new \Exception('CiviCRM API error (Activity): ' . $res->error_message);
  }

  // Add note.
  if (!empty($data['note'])) {
    spaanmelden_create_note($contact_id, $data['note']);
  }

  return $contact_id;
}

/**
 * Description missing.
 */
function memberform_membership_description($membership_type, $short = FALSE) {
  switch ($membership_type) {
    case 'lid_sp':
      return ($short ? 'SP' : 'lid van de SP');

    case 'lid_sprood':
      return ($short ? 'SP + ROOD' : 'lid van de SP en ROOD');

    case 'lid_rood':
      return ($short ? 'ROOD' : 'lid van ROOD');

    case 'tribune':
      return ($short ? 'Tribune' : 'abonnee van de Tribune');

    case 'spanning':
      return ($short ? 'Spanning' : 'abonnee van Spanning');

    default:
      return '[' . ($short ? 'onbekend' : 'onbekend lidmaatschap') . ']';
  }
}

/**
 * Description missing.
 */
function memberform_get_gift_internal_name($gift_id) {
  return db_select('memberform_gifts', 'sg')
    ->fields('sg', ['internal_name'])
    ->condition('id', $gift_id, '=')
    ->execute()
    ->fetchField(0);
}
