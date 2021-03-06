<?php

/**
 * @file
 * Contains the MembershipEntity class.
 */

/**
 * Defines a membership entity.
 */
class MembershipEntityTerm extends Entity {
  /**
   * The primary term identifier.
   *
   * @var int
   */
  public $id;

  /**
   * The membership identifier.
   *
   * @var int
   */
  public $mid;

  /**
   * The human readable term length. eg. 1 year.
   *
   * @var string
   */
  public $term;

  /**
   * An array term length modifiers. eg. +1 month.
   *
   * @var array
   */
  public $modifiers;

  /**
   * The date when this term starts.
   *
   * @var string
   */
  public $start;

  /**
   * The date when this term ends.
   *
   * @var string
   */
  public $end;

  /**
   * The Unix timestamp when the membership term was created.
   *
   * @var int
   */
  public $created;

  /**
   * The Unix timestamp when the membership term was most recently saved.
   *
   * @var int
   */
  public $changed;

  /**
   * Build a new membership entity term.
   */
  public function __construct($values = array()) {
    return parent::__construct($values, 'membership_entity_term');
  }

  /**
   * Returns the Membership Entity.
   */
  public function membership() {
    return membership_entity_load($this->mid);
  }

  /**
   * Sets the Membership Entity.
   *
   * @param mixed $membership
   *   The Membership Entity object or the Membership Entity id (mid).
   */
  public function setMembership($membership) {
    $this->mid = is_object($membership) ? $membership->mid : $membership;
  }

  /**
   * Returns the full url to the membership term.
   */
  public function url() {
    $uri = $this->uri();
    return url($uri['path'], $uri);
  }

  /**
   * Returns the Drupal path to the membership term.
   */
  public function path() {
    $uri = $this->uri();
    return $uri['path'];
  }

  /**
   * Overrides Entity::defaultUri().
   */
  public function defaultUri() {
    return array('path' => "membership/$this->mid/term/$this->id");
  }

  /**
   * Overrides Entity::save().
   */
  public function save() {
    // Check for valid term length that is understood by DateTime::modify().
    if (preg_match(MEMBERSHIP_ENTITY_TERM_LENGTH_REGEX, $this->term)) {
      $this->calculateEndDate();
    }
    elseif ($this->term == 'lifetime') {
      $this->end = NULL;
    }

    // Update created and changed dates.
    if (empty($this->created)) {
      $this->created = REQUEST_TIME;
    }
    $this->changed = REQUEST_TIME;

    return parent::save();
  }

  /**
   * Calculate the term end date.
   */
  protected function calculateEndDate() {
    // Add the term length.
    $end = new DateObject($this->start, new DateTimezone($this->timezone));
    $end = _membership_entity_term_modify_date($end, $this->term);

    // Add modifiers.
    foreach ($this->modifiers as $modifier) {
      $end = _membership_entity_term_modify_date($end, $modifier);
    }

    // Allow other modules to override the end date.
    drupal_alter('membership_entity_term_end_date', $end, $this);

    $this->end = date_format_date($end, 'custom', DATE_FORMAT_DATETIME);
  }

}
