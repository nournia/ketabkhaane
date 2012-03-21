-- 0 <= rate <= 1

/*
	auto_increment -> autoincrement
	cascade -> cascade deferrable initially deferred
*/

-- tables ------------------------------------------------------------------------------

-- globals 
create table ageclasses(
	id tinyint(4) not null primary key,
	title varchar(255) not null,
	description varchar(255) null default null,
	beginage tinyint(4) not null,
	endage tinyint(4) not null,
	questions tinyint(4) not null
);
create table categories (
	id tinyint(4) not null primary key,
	title varchar(255) not null
);

-- users
create table users (
	id integer not null primary key autoincrement,
	national_id integer null default null,
	firstname varchar(255) not null,
	lastname varchar(255) not null,
	birth_date date null default null,
	address varchar(255) null default null,
	phone varchar(50) null default null,
	gender varchar(10) not null, -- enum("male","female")
	description varchar(255) null default null,
	email varchar(255) null default null,
	upassword char(40) null default null,
	label varchar(10) null default null, 

	score integer not null default "0",
	correction_time integer not null default "0",

	unique (email) on conflict abort,
	unique (national_id) on conflict abort
);
create table permissions (
	id integer not null primary key autoincrement,
	user_id integer not null references users(id) on update cascade,
	permission varchar(10) not null, -- enum("user", "operator", "designer", "manager", "master", "admin")
	accept tinyint(1) not null default "0"
);

-- matches
create table authors (
	id integer not null primary key autoincrement,
	title varchar(255) not null
);
create table publications (
	id integer not null primary key autoincrement,
	title varchar(255) not null
);
create table files (
	id integer not null primary key autoincrement,
	extension varchar(5) not null
);
create table resources (
	id integer not null primary key autoincrement,
	author_id integer null default null references authors(id) on update cascade,
	publication_id integer null default null references publications(id) on update cascade,
	kind varchar(12) not null, -- enum("book", "multimedia", "webpage")
	-- tags set("") null default null,
	title varchar(255) not null,
	ageclass tinyint(4) null default null,
	content text null default null, -- html for all types
	link varchar(1000) null default null, -- book:, multimedia: fileaddress, webpage: url
	volume int null default null -- book: pages, multimedia: seconds, webpage: words
);
create table matches (
	id integer not null primary key autoincrement,
	designer_id integer null default null references users(id) on update cascade,

	title varchar(255) not null,
	ageclass tinyint(4) null default null,
	
	-- question
	resource_id integer null default null references resources(id) on update cascade,
	
	-- instruction
	category_id tinyint(4) null default null references categories(id) on update cascade,
	content text null default null
);
create table questions (
	id integer not null primary key autoincrement,
	match_id integer not null references matches(id) on update cascade,
	question varchar(1000) not null,
	answer varchar(1000) null default null
	--choice tinyint(4) null default null -- null: no choice
);

-- answers
create table answers (
	id integer not null primary key autoincrement,
	user_id integer not null references users(id) on update cascade,
	match_id integer not null references matches(id) on update cascade,
	delivered_at datetime not null default current_timestamp,
	received_at datetime null default null,
	corrected_at datetime null default null,
	rate float null default null
);

-- tournaments 
create table library (
	id integer null default null,
	group_id integer not null,
	title varchar(255) not null,
	description varchar(1000) null default null,
	synced_at timestamp null default null,
	license varchar(255) null default null,
	image varchar(50) null default null,

	-- tournament info
	tournament_title varchar(255) not null,
	started_at datetime not null,
	designer_coeff float not null default "0",
	pay_coeff float not null default "1",
	pay_unit varchar(100) not null default "امتیاز",
	active tinyint(1) not null default "1",
	open_user tinyint(1) not null default "1",
	version varchar(10) null,
	options text null
);
create table supports (
	id integer not null primary key autoincrement,
	match_id integer not null references matches(id) on update cascade,
	corrector_id integer not null references users(id) on update cascade,
	current_state varchar(10) not null, -- enum("active", "disabled", "imported")
	score smallint
);
create table scores (
	id integer not null primary key autoincrement,
	user_id integer not null,
	score integer not null default "0",
	participated_at datetime not null default current_timestamp,
	confirm tinyint(1) not null default "1"
);
create table payments (
	id integer not null primary key autoincrement,
	user_id integer not null references users(id) on update cascade,
	payment smallint not null,
	payed_at timestamp not null default current_timestamp
);

-- open_scores 
create table open_categories (
	id integer not null primary key autoincrement,
	title varchar(255) not null
);
create table open_scores (
	id integer not null primary key autoincrement,
	user_id integer not null references users(id) on update cascade,
	category_id tinyint(4) not null references categories(id) on update cascade,
	title varchar(255) not null,
	score smallint(6) not null
);


-- library
create table roots (
	id integer not null primary key autoincrement,
	title varchar(255) not null
);
create table branches (
	id integer not null primary key autoincrement,
	root_id integer not null,
	title varchar(255) not null,
	label varchar(10) null default null
);
create table objects (
	id integer not null primary key autoincrement,
	resource_id integer not null references resources(id) on update cascade,
	branch_id integer not null,
	label varchar(50) not null,
	cnt int not null default 0 -- count of object in this library
);

-- log
create table logs (
	table_name varchar(20) not null,
	row_op varchar(10) not null, -- enum("insert","update", "delete")
	row_id integer not null,
	row_data text null,
	user_id integer null references users(id) on update cascade,
	created_at timestamp default current_timestamp
);


-- data ------------------------------------------------------------------------------

insert into ageclasses values (0, 'الف', 'آمادگی و اول دبستان', 6, 7, 4);
insert into ageclasses values (1, 'ب', 'دوم و سوم دبستان', 8, 9, 4);
insert into ageclasses values (2, 'ج', 'چهارم و پنجم دبستان', 10, 11, 5);
insert into ageclasses values (3, 'د', 'راهنمایی', 12, 14, 6); 
insert into ageclasses values (4, 'ه', 'دبیرستان', 15, 18, 7);

insert into categories (id, title) values (0, 'نقاشی');
insert into categories (id, title) values (1, 'رنگ‌آمیزی');
insert into categories (id, title) values (2, 'تحقیق');
insert into categories (id, title) values (3, 'آزمایش');
insert into categories (id, title) values (4, 'کاردستی');

insert into open_categories (id, title) values (0, 'خلاصه‌نویسی');
insert into open_categories (id, title) values (1, 'شعر');
insert into open_categories (id, title) values (2, 'داستان');

insert into library (id, group_id, title, image, license, tournament_title, started_at, version) values (1, 1, 'کتابخانه‌ی شهید خرازی', '1.jpg','aslwkelrfjsasdf', 'مسابقه کتاب‌خوانی', '2011-06-01 00:00:00', '0.8.5');
insert into permissions (user_id, permission, accept) values (1770, "master", 1);

-- after import
update users set upassword = '356a192b7913b04c54574d18c28d46e6395428ab' where id = 1770;
update scores set score = 0;