# BugCoder

## エンドポイント

```
# ソースコード全部
/api/v1/code

# 1個
/api/v1/code/:id
```

### ログインの post

```
curl -X POST -d "user_name=a&password=pass" http://localhost:5050/api/v1/user/login
```
