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
create table payments (
	id integer not null primary key autoincrement,
	user_id integer not null references users(id) on update cascade,
	payment smallint not null, -- positive: rise of user score
	payed_at timestamp not null default current_timestamp,
	kind varchar(10) not null, -- enum("match", "payment", "penalty", "receipt") -- effect: +, -, -, + 
	reference integer null -- match: match_id, penalty: object_id
);


create table roots (
	id integer not null primary key autoincrement,
	title varchar(255) not null
);
create table branches (
	id integer not null primary key autoincrement,
	root_id integer not null references roots(id) on update cascade,
	title varchar(255) not null,
	label varchar(10) null default null
);
create table objects (
	id integer not null primary key autoincrement,
	resource_id integer not null references resources(id) on update cascade,
	branch_id integer not null references branches(id) on update cascade,
	label varchar(50) not null,
	cnt int not null default 0 -- count of object in this library
);
create table borrows (
	id integer not null primary key autoincrement,
	user_id integer not null references users(id) on update cascade,
	object_id integer not null references resources(id) on update cascade,
	delivered_at datetime not null default current_timestamp,
	received_at datetime null default null
);

