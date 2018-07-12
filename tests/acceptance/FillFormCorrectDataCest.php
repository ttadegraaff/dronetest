<?php


class FillFormCorrectDataCest
{
  public function _before(AcceptanceTester $I)
  {
  }

  public function _after(AcceptanceTester $I)
  {
  }

  // tests
  /*
  public function finishYourself(AcceptanceTester $I)
  {
    $I->wantTo('To complete the form with valid data.');
    $I->amOnpage('aanmelden/lid');
    $I->seeInTitle('Word lid van de SP');
    $I->see('Contactgegevens');
    $I->fillField('Naam', 'Boris van de Bakvet');
    $I->fillField('E-mailadres', 'bbakvet@sp.nl');
    $I->fillField('Telefoonnummer', '0612345678');
    $I->click('Verder');
    $I->waitForElement('[value="Ik vul mijn gegevens meteen zelf in"]', 60);
    $I->seeElement('input', ['value' => 'Ik vul mijn gegevens meteen zelf in']);
    $I->seeElement('input', ['value' => 'Bel me om lid te worden']);
    $I->click('Bel me om lid te worden');
    $I->waitForText('Bedankt');
    $I->see('Bedankt', 'h3');
  }
  */
  public function callBack(AcceptanceTester $I)
  {
    // Contactgegevens
    $I->wantTo('To complete the form with valid data.');
    $I->amOnpage('aanmelden/lid');
    $I->seeInTitle('Word lid van de SP');
    $I->see('Contactgegevens');
    $I->fillField('Naam', 'Boris van de Bakvet');
    $I->fillField('E-mailadres', 'bbakvet@sp.nl');
    $I->fillField('Telefoonnummer', '0612345678');
    $I->click('Verder');
    // Bel me
    $I->waitForElement('[value="Ik vul mijn gegevens meteen zelf in"]', 60);
    $I->seeElement('input', ['value' => 'Bel me om lid te worden']);
    $I->click('Ik vul mijn gegevens meteen zelf in');
    // Persoonsgegevens
    $I->waitForText('PERSOONSGEGEVENS');
    $I->seeInField('Voornaam', 'Boris');
    $I->seeInField('Tussenvoegsel', 'van de');
    $I->seeInField('Achternaam', 'Bakvet');
    $I->selectOption('Dag', '1');
    $I->selectOption('Maand', '1');
    $I->selectOption('Jaar', '2001');
    $I->click('Verder');
    // ROOD lidmaatschap
    $I->waitForText('ROOD LIDMAATSCHAP');
    $I->selectOption('form input[name="page_red_membership"]', 'sp_and_red_member');
    $I->click('Verder');
    // Adresgegevens
    $I->waitForText('ADRESGEGEVENS');
    $I->fillField('Postcode', '3813PK');
    $I->fillField('Huisnummer', '99');
    $I->fillField('Huisnummer toevoeging', '');
    $I->waitForElement('[value="Camera Obscurastraat"]');
    $I->waitForElement('[value="AMERSFOORT"]');
    $I->click('Verder');
    // Persoonlijke bijdrage
    $I->waitForText('PERSOONLIJKE BIJDRAGE');
    $I->executeJS('jQuery("input[name=\'page_contribution_choice\']").show()');
    $I->selectOption('form input[name="page_contribution_choice"]', 'custom');
    $I->waitForText('Kies een bedrag (minimaal 5 euro)');
    $I->fillField('Kies een bedrag (minimaal 5 euro)', '33');
    $I->executeJS('jQuery("input[name=\'page_contribution_payment_method\']").show()');
    $I->selectOption('form input[name="page_contribution_payment_method"]', 'auto');
    $I->wait(1);
    $I->fillField('Wat is je bankrekeningnummer?', 'NL32ASNB0708595189');
    $I->click('Verder');
    // Welkomstgeschenk
    $I->waitForText('WELKOMSTGESCHENK');
    $I->selectOption('Welkomstgeschenk ROOD', 'Ik hoef geen welkomstcadeau');
    $I->executeJS('jQuery("#edit-page-gift-select-lid-sp-5").show()');
    $I->selectOption('#edit-page-gift-select-lid-sp-5', '5');
    $I->wait(1);
    $I->click('Verder');
    // Controleer uw gegevens
    $I->waitForText('CONTROLEER UW GEGEVENS');
    $I->see('Boris van de Bakvet');
    $I->see('bbakvet@sp.nl');
    $I->see('0612345678');
    $I->see('01-01-2001');
    $I->see('3813PK');
    $I->see('99');
    $I->see('Camera Obscurastraat');
    $I->see('AMERSFOORT');
    $I->see('Anders');
    $I->see('33,00');
    $I->see('Automatische incasso');
    $I->see('NL32ASNB0708595189');
    $I->see('Ik hoef geen welkomstcadeau');
    $I->see('ROOD en SP');
    $I->click('Insturen');
    // Feedback
    $I->waitForText('BEDANKT');
    $I->see('Bedankt Boris voor je aanmelding als lid van de SP. Samen werken we aan ons sociaal alternatief.');
  }
}
