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
	account smallint not null references accounts(id) on update cascade,

	unique (email) on conflict abort,
	unique (national_id) on conflict abort
);
create table transactions (
	id integer not null primary key autoincrement,
	user_id integer not null references users(id) on update cascade,
	score smallint not null, -- match: +answer -payment, library: +receipt -penalty
	created_at timestamp not null default current_timestamp,
	description varchar(20) null -- off: discount (fine from objects), chg (money user charged to he's account), mid:match_id (score from match), pay (money payed to user for matches)
);


create table types (
	id integer not null primary key autoincrement,
	title varchar(50) not null
);
create table accounts (
	id integer not null primary key autoincrement,
	title varchar(255) not null,
	bookfine integer not null, -- daily after one week
	cdfine integer not null -- daily
);
create table roots (
	id integer not null primary key autoincrement,
	title varchar(255) not null,
	type_id smallint not null references types(id) on update cascade
);
create table branches (
	id integer not null primary key autoincrement,
	root_id integer not null references roots(id) on update cascade,
	title varchar(255) not null,
	label varchar(10) null default null
);
create table objects (
	id integer not null primary key autoincrement,
	author_id integer null default null references authors(id) on update cascade,
	publication_id integer null default null references publications(id) on update cascade,
	type_id smallint not null references types(id) on update cascade,
	title varchar(255) not null,
	branch_id integer not null references branches(id) on update cascade,
	label varchar(50) not null,
	cnt int not null default 0 -- count of object in this library
);
create table borrows (
	id integer not null primary key autoincrement,
	user_id integer not null references users(id) on update cascade,
	object_id integer not null references objects(id) on update cascade,
	delivered_at datetime not null default current_timestamp,
	received_at datetime null default null,
	renewed_at datetime null default null
);

insert into accounts (id, title, bookfine, cdfine) values (0, 'عادی', 50, 100);
insert into accounts (id, title, bookfine, cdfine) values (1, 'ویژه', 25, 100);

insert into types (id, title) values (0, 'کتاب');
insert into types (id, title) values (1, 'چند رسانه‌ای');
