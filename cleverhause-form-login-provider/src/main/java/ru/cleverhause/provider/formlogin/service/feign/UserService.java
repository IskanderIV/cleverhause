package ru.cleverhause.provider.formlogin.service.feign;

import org.springframework.cloud.openfeign.FeignClient;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;

@FeignClient(name ="${feign.clients.users.name}", url="${feign.clients.users.url}")
public interface UserService {

    @GetMapping(path = "/api/users/{username}")
    UserDetails getUserByUsername(@PathVariable String username);
}