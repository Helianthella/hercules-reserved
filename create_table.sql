-- creates the `char_reservation` table.
-- only the `name` field is used so you can safely
-- add your own fields.

CREATE TABLE IF NOT EXISTS `char_reservation` (
  `name` varchar(30) NOT NULL, -- same length as in the `char` table
  PRIMARY KEY (`name`)
) ENGINE=InnoDB;
