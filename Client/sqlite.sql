create table users (
        id integer not null primary key autoincrement,
        nationalid integer null default null,
        quality integer not null default "0",
        firstname varchar(255) not null,
        lastname varchar(255) not null,
        birth_date date not null,
        address varchar(255) null default null,
        phone varchar(50) null default null,
        gender integer not null,
        description varchar(255) null default null,
        email varchar(255) default null,
        upassword char(40) default null,

        created_at datetime null default null,
        score integer not null default "0",
        correction_time integer not null default "0",

        unique (email) on conflict abort,
        unique (nationalid) on conflict abort
);
