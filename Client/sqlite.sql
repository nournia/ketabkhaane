-- 0 <= rate <= 1, 0 <= quality

/*
	int(11) -> integer
	auto_increment -> autoincrement
	primary key -> in field
*/

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

-- users
create table users (
	id integer not null primary key autoincrement,
	national_id integer null default null,
	quality integer not null default "0",
	firstname varchar(255) not null,
	lastname varchar(255) not null,
	birth_date date null default null,
	address varchar(255) null default null,
	phone varchar(50) null default null,
	gender varchar(10) not null, -- enum("male","female")
	description varchar(255) null default null,
	email varchar(255) default null,
	upassword char(40) default null,

	score integer not null default "0",
	correction_time integer not null default "0",

	created_at timestamp default current_timestamp,
	updated_at timestamp default current_timestamp, 
	unique (email) on conflict abort,
	unique (national_id) on conflict abort
);
create table library (
	-- group 
	title varchar(255) not null,
	description varchar(1000) null default null,
  
	sync_time datetime null default null,
	-- library 
	-- uniqueid char(40) not null,
	-- serverid char(32) null default null,
	licence varchar(255) null default null
);
create table pictures (
	id integer not null primary key autoincrement,
	reference_id integer not null,
	kind varchar(10) not null, -- enum("library", "user", "resource", "match")
	picture blob null,

	created_at timestamp default current_timestamp,
	updated_at timestamp default current_timestamp
);

-- matches
create table authors (
	id integer not null primary key autoincrement,
	quality integer not null default "0",
	title varchar(255) not null,

	created_at timestamp default current_timestamp,
	updated_at timestamp default current_timestamp
);
create table publications (
	id integer not null primary key autoincrement,
	quality integer not null default "0",
	title varchar(255) not null,

	created_at timestamp default current_timestamp,
	updated_at timestamp default current_timestamp
);
create table resources (
	id integer not null primary key autoincrement,
	author_id integer null default null,
	publication_id integer null default null,
	entity_id integer null default null, -- books.id | multimedias.id | webpages.id 
	quality integer not null default "0",
	kind varchar(12) not null, -- enum("book", "multimedia", "webpage")
	-- tags set("") null default null,
	title varchar(255) not null,
	ageclass tinyint(4) null default null,

	created_at timestamp default current_timestamp,
	updated_at timestamp default current_timestamp
);
create table matches (
	id integer not null primary key autoincrement,
	designer_id integer not null, -- user_id
	quality integer not null default "0",

	title varchar(255) not null,
	ageclass tinyint(4) null default null,
	
	-- question
	resource_id integer null default null,
	
	-- instruction
	category_id tinyint(4) null default null,
	content text null default null,
	configuration varchar(50) null default null,

	created_at timestamp default current_timestamp,
	updated_at timestamp default current_timestamp
);
create table questions (
	id integer not null primary key autoincrement,
	match_id integer not null,
	question varchar(1000) not null,
	answer varchar(1000) null default null,
	--choice tinyint(4) null default null -- null: no choice

	created_at timestamp default current_timestamp,
	updated_at timestamp default current_timestamp
);

-- answers
create table answers (
	id integer not null primary key autoincrement, -- local
	user_id integer not null,
	match_id integer not null,
	received_at datetime null default null,
	corrected_at datetime null default null,
	rate float null default null,

	created_at timestamp default current_timestamp,
	updated_at timestamp default current_timestamp
);

-- tournaments 
  create table supports (
	id integer not null primary key autoincrement, -- local
	match_id integer not null,
	corrector_id integer not null, -- user_id
	score smallint,
	current_state varchar(10) not null, -- enum("active", "disabled", "imported")

	created_at timestamp default current_timestamp,
	updated_at timestamp default current_timestamp
);
create table payments (
	id integer not null primary key autoincrement,
	user_id integer not null,
	payment smallint not null,

	created_at timestamp default current_timestamp,
	updated_at timestamp default current_timestamp
);

-- open_scores 
create table open_categories (
	id integer not null primary key autoincrement,
	title varchar(255) not null
);
create table open_scores (
	id integer not null primary key autoincrement,
	user_id integer not null,
	category_id tinyint(4) not null,
	title varchar(255) not null,
	score smallint(6) not null,

	created_at timestamp default current_timestamp,
	updated_at timestamp default current_timestamp
);



-- data ------------------------------------------------------------------------------

insert into ageclasses values (0, 'الف', 'آمادگی و سال اول دبستان', 6, 7);
insert into ageclasses values (1, 'ب', 'سال‌های دوم و سوم دبستان', 8, 9);
insert into ageclasses values (2, 'ج', 'سال‌های چهارم و پنجم دبستان', 10, 11);
insert into ageclasses values (3, 'د', 'سال‌های راهنمایی', 12, 14); 
insert into ageclasses values (4, 'ه', 'سال‌های دبیرستان', 15, 18);

insert into categories (id, title) values (0, 'نقاشی');
insert into categories (id, title) values (1, 'رنگ‌آمیزی');
insert into categories (id, title) values (2, 'تحقیق');
insert into categories (id, title) values (3, 'آزمایش');
insert into categories (id, title) values (4, 'کاردستی');

insert into open_categories (id, title) values (0, 'خلاصه‌نویسی');
insert into open_categories (id, title) values (1, 'شعر');
insert into open_categories (id, title) values (2, 'داستان');

insert into library (title) values ('کتابخانه‌ی شهید خرازی');
