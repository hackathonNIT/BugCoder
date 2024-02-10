INSERT INTO codes (user_id, parent_id, code_data, title, detail, lang_id)
VALUES
    (1, NULL, '#include<iostream>\nint main(){\n cout<<"hello"<<endl;\n}', 'hello出ない', 'C++でhelloを出したいのですが、なんかエラー出てます。\nどうにかしてくださいよ',1),
    (2, 4, '#include<iostream>\nint main(){\n cout<<"hello"<<endl;\n return0;\n}', 'hello出ない', 'return 0がないのでは？？',1),
    (2, 4, '#include<iostream>\nint main(){\n std::cout<<"hello"<<std::endl;\n return0;\n}', 'hello出ない', 'stdの名前空間がありません',1);