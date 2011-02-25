﻿-- 0 <= rate <= 1, 0 <= quality	

/*
	int(11) -> integer
	auto_increment -> autoincrement
	primary key -> in field
*/

-- tables ------------------------------------------------------------------------------

-- globals 

create table ageclasses(
	id tinyint(4) not null,
	title varchar(255) not null,
	description varchar(255) null default null,
	beginage tinyint(4) not null,
	endage tinyint(4) not null,
	primary key (id)
);


-- users
create table users (
	id integer not null primary key autoincrement,
	nationalid integer null default null,
	quality integer not null default "0",
	firstname varchar(255) not null,
	lastname varchar(255) not null,
	birth_date date not null,
	address varchar(255) null default null,
	phone varchar(50) null default null,
	gender boolean not null,
	description varchar(255) null default null,
	email varchar(255) default null,
	upassword char(40) default null,

	created_at datetime null default null,
	score integer not null default "0",
	correction_time integer not null default "0",

	unique (email) on conflict abort,
	unique (nationalid) on conflict abort
);

-- matches
create table authors (
	id integer not null primary key autoincrement,
	quality integer not null default "0",
	title varchar(255) not null
);
create table publications (
	id integer not null primary key autoincrement,
	quality integer not null default "0",
	title varchar(255) not null
);

-- answers

-- tournaments 
 

-- open_scores 
 

-- data ------------------------------------------------------------------------------

insert into ageclasses values (0, 'الف', 'آمادگی و سال اول دبستان', 6, 7);
insert into ageclasses values (1, 'ب', 'سال‌های دوم و سوم دبستان', 8, 9);
insert into ageclasses values (2, 'ج', 'سال‌های چهارم و پنجم دبستان', 10, 11);
insert into ageclasses values (3, 'د', 'سال‌های راهنمایی', 12, 14); 
insert into ageclasses values (4, 'ه', 'سال‌های دبیرستان', 15, 18);
