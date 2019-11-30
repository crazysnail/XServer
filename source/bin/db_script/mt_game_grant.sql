use mysql;
GRANT USAGE ON *.* TO 'mt_game'@'localhost' IDENTIFIED BY '123456' WITH GRANT OPTION;
GRANT ALL PRIVILEGES ON `mt_game_%`.* TO 'mt_game'@'localhost' IDENTIFIED BY '123456';
GRANT USAGE ON *.* TO 'moqidb'@'115.159.227.158' IDENTIFIED BY 'shnzgsl128zns' WITH GRANT OPTION;
GRANT select ,create temporary tables ON `mt_game_%`.* TO 'moqidb'@'115.159.227.158' IDENTIFIED BY 'shnzgsl128zns';
GRANT USAGE ON *.* TO 'mt_web_sql'@'localhost' IDENTIFIED BY 'shnzgsl128zns' WITH GRANT OPTION;
GRANT select ,create temporary tables ON `mt_game_%`.* TO 'mt_web_sql'@'localhost' IDENTIFIED BY 'shnzgsl128zns';
FLUSH PRIVILEGES;
