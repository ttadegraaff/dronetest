<?php
$I = new AcceptanceTester($scenario);
$I->wantTo('perform actions and see result');
$I->amOnpage('aanmelden/lid');
$I->submitForm('form.memberform_membership', []);
$I->see('Dit veld is verplicht!');
