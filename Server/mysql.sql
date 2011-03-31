drop database if exists reghaabat_development;
create database reghaabat_development character set utf8 collate utf8_general_ci;
use reghaabat_development;

-- 0 <= rate <= 1, 0 <= quality	

-- tables ------------------------------------------------------------------------------

-- globals 
create table ageclasses(
	id tinyint(4) not null primary key,
	title varchar(255) not null,
	description varchar(255) null default null,
	beginage tinyint(4) not null,
	endage tinyint(4) not null
);
create table categories (
	id tinyint(4) not null primary key,
	title varchar(255) not null
);
/*create table tags (
	id tinyint(4) not null,
	title varchar(50) not null,
	primary key (id)
);*/

-- users
create table users (
	id int not null primary key auto_increment,
	national_id int null default null,
	-- quality int not null default "0",
	firstname varchar(255) not null,
	lastname varchar(255) not null,
	birth_date date null default null,
	address varchar(255) null default null,
	phone varchar(50) null default null,
	gender enum("male","female") not null,
	description varchar(255) null default null,
	email varchar(255) default null collate "ascii_bin",
	upassword char(40) default null collate "ascii_bin",

	score int not null default "0",
	correction_time int not null default "0" comment "minute",
	
	created_at timestamp null default null, 
	updated_at timestamp null default null,
	unique key email (email),
	unique key nationalid (national_id)
);
create table groups (
	id integer not null primary key auto_increment, 
	master_id integer not null,
	title varchar(255) not null,
	description varchar(1000) default null
);
create table libraries (
	id integer not null primary key auto_increment,
	group_id integer not null,
	tournament_id integer not null,
	title varchar(255) not null,
	description varchar(1000) null default null,
	synced_at timestamp null default null,
	license varchar(255) null default null
);
create table permissions (
	id integer not null primary key auto_increment,
	group_id integer not null,
	user_id integer not null,
	permission enum("user", "operator", "designer", "manager", "master", "admin") not null, -- ozv, ozvyar, tarrah, tarrahyar, modir, modir-e-samaneh 
	accept tinyint(1) not null default "0",

	created_at timestamp null default null, 
	updated_at timestamp null default null
);
create table pictures (
	id integer not null primary key auto_increment,
	reference_id integer not null,
	kind enum("library", "user", "resource", "match") not null,
	picture mediumblob null,

	created_at timestamp null default null, 
	updated_at timestamp null default null
);

-- matches 
create table authors (
	id integer not null primary key auto_increment,
	-- quality integer not null default "0",
	title varchar(255) not null,

	created_at timestamp null default null, 
	updated_at timestamp null default null
);
create table publications (
	id integer not null primary key auto_increment,
	-- quality integer not null default "0",
	title varchar(255) not null,

	created_at timestamp null default null, 
	updated_at timestamp null default null
);
create table resources (
	id integer not null primary key auto_increment,
	author_id integer null default null,
	publication_id integer null default null,
	-- quality integer not null default "0",
	kind enum("book", "multimedia", "webpage") not null,
	-- tags set("") null default null,
	title varchar(255) not null,
	ageclass tinyint(4) null default null,
	content text null default null, -- html for all types
	link varchar(1000) null default null collate "ascii_bin", -- book:, multimedia: fileaddress, webpage: url
	volume int null default null, -- book: pages, multimedia: seconds, webpage: words

	created_at timestamp null default null, 
	updated_at timestamp null default null
);
create table matches (
	id integer not null primary key auto_increment,
	designer_id integer not null,
	-- quality integer not null default "0",

	title varchar(255) not null,
	ageclass tinyint(4) null default null,
	
	-- question
	resource_id integer null default null,
	
	-- instruction
	category_id tinyint(4) null default null,
	content text null default null,
	configuration varchar(50) null default null,

	created_at timestamp null default null, 
	updated_at timestamp null default null
);
create table questions (
	id integer not null primary key auto_increment,
	match_id integer not null,
	question varchar(1000) not null,
	answer varchar(1000) null default null,
	-- choice tinyint(4) null default null, -- null: no choice

	created_at timestamp null default null, 
	updated_at timestamp null default null
);
/*create table choices (
	id integer not null auto_increment,
	questionid tinyint(4) not null,
	choice varchar(255) default null,
	primary key (id),
	foreign key (questionid) references questions(id)
);*/

-- answers 
create table answers (
	id integer not null primary key auto_increment,
	user_id integer not null,
	match_id integer not null,
	received_at datetime null default null,
	corrected_at datetime null default null,
	rate float null default null,

	created_at timestamp null default null, 
	updated_at timestamp null default null
);
/*create table subanswers (
	id integer not null auto_increment,
	answerid integer not null,
	question varchar(1000) not null,
	answer varchar(1000) not null,
	rate float default null,
	message text comment "designer message to user",
	attachment blob,
	primary key (id),
	foreign key (answerid) references answers(id)
);

-- tournaments 
create table tournaments (
	id integer not null auto_increment,
	title varchar(255) not null,
	starttime datetime not null,
	active tinyint(1) not null,
	creatorid integer not null,
	userpaytransform float not null,
	designerpaytransform float not null,
	openuser tinyint(1) not null default "1",
	address varchar(1000) default null,
	payunit varchar(100) not null,
	primary key (id)
);
create table follows (
	id integer not null auto_increment,
	tournamentid integer not null,
	followedid integer not null,
	primary key (id),
	foreign key (tournamentid) references tournaments(id),
	foreign key (followedid) references tournaments(id)
);*/
create table supports (
	id integer not null primary key auto_increment,
	tournament_id integer not null,
	match_id integer not null,
	corrector_id integer not null,
	current_state enum("active", "disabled", "imported") not null,
	score smallint(6),

	created_at timestamp null default null, 
	updated_at timestamp null default null
);
create table scores (
	id integer not null primary key auto_increment,
	tournament_id integer not null,
	user_id integer not null,
	score integer not null default "0",
	participated_at datetime not null,
	confirm tinyint(1) not null default "1",

	created_at timestamp null default null, 
	updated_at timestamp null default null
);
create table payments (
	id integer not null primary key auto_increment,
	tournament_id integer not null,
	user_id integer not null,
	payment smallint(6) not null,

	created_at timestamp null default null, 
	updated_at timestamp null default null
);

-- open_scores 
create table open_categories (
	id integer not null primary key auto_increment,
	title varchar(255) not null
);
create table open_scores (
	id integer not null primary key auto_increment,
	tournament_id integer not null,
	user_id integer not null,
	category_id tinyint(4) not null,
	title varchar(255) not null,
	score smallint(6) not null,

	created_at timestamp null default null, 
	updated_at timestamp null default null
);

-- data ------------------------------------------------------------------------------

insert into ageclasses values (0, 'الف', 'آمادگی و سال اول دبستان', 6, 7);
insert into ageclasses values (1, 'ب', 'سال‌های دوم و سوم دبستان', 8, 9);
insert into ageclasses values (2, 'ج', 'سال‌های چهارم و پنجم دبستان', 10, 11);
insert into ageclasses values (3, 'د', 'سال‌های راهنمایی', 12, 14); 
insert into ageclasses values (4, 'ه', 'سال‌های دبیرستان', 15, 18);

insert into libraries (id, group_id, tournament_id, title, license) values (1, 1, 1, 'کتابخانه‌ی شهید خرازی', 'aslwkelrfjsasdf');
insert into groups (id, master_id, title) values (1, 1, 'شهید خرازی');
