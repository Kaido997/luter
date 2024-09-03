import { Body, Controller, Delete, Get, Param, Patch, Post, Query } from "@nestjs/common";
import { ApiBody, ApiOperation, ApiParam, ApiProperty, ApiQuery, ApiResponse, ApiTags } from "@nestjs/swagger";
import { BaseAPIResponseDTO } from "src/classes/base-response";
import { RequireAuth } from "src/core/auth/shared/decorators/authorization.decorator";
import { JwtAuthGuard } from "src/core/auth/strategies/jwt/jwt.guard";
import { Session_Schema } from "./schemas/session.schema";
import { RoleAuthGuard } from "src/core/auth/strategies/auth-role/auth-role.guard";
import { AuthorizedRoles } from "src/core/auth/shared/decorators/authorized-role.decorator";
import { allRoles, managers } from "../role/types/roles.enum";
import { SessionService } from "./session.service";
import { CompanyConfig_Schema } from "../company/submodules/config/schemas/company-config.schema";
import { SessionPWAResponseDTO } from "./classes/get-session-pwa.dto";
import { PostSessionDTO, PostSessionResponseDTO } from "./classes/post-session.dto";
import { AuthUser } from "src/core/tools/decorators/user.decorator";
import { User_Schema } from "../user/schemas/user.schema";
import { CompanyService } from "../company/services/company.service";
import { Company_Schema } from "../company/schemas/company.schema";
import { ObjectId } from "bson";
import { GetAllSessionDTO } from "./classes/get-session.dto";
import { getSessionStatusRef, SessionStatus, SessionStatusRef } from "./types/session-status";
import { ConfigResponseDTO } from "../company/submodules/config/classes/company-config.dto";
import { EditSessionDTO } from "./classes/edit-session.dto";
import { GetVideoURLDTO } from "./classes/get-video.dto";
import { JouleService } from "../joule/joule.service";
import { isNotEmpty } from "class-validator";

@Controller("/session")
@ApiTags('session')
export class SessionController {

  public constructor(
    private readonly sessionService: SessionService,
    private readonly companyService: CompanyService,
    private readonly jService: JouleService
  ) { }

  @Get("/:token/status")
  @ApiResponse({ status: 200, type: SessionPWAResponseDTO })
  private async _getSessionStatus(@Param("token") token: string): Promise<SessionPWAResponseDTO> {
    const respponse: SessionPWAResponseDTO = new SessionPWAResponseDTO();
    try {
      const session: Session_Schema = await this.sessionService.findByToken(token);
      if (!session) {
        respponse.messages.push("SESSION_NOT_FOUND");
        return respponse;
      };
      respponse.setSession(session);
    } catch (error) {
      respponse.messages.push(error?.message || "UNEXPECTED_ERROR_ON_SESSION_STATUS_RETRIEVAL");
    }
    return respponse;
  }

  @Get("/:token/config")
  @ApiResponse({ status: 200, type: SessionPWAResponseDTO })
  private async _getSessionConfig(@Param("token") token: string): Promise<ConfigResponseDTO> {
    const response: ConfigResponseDTO = new ConfigResponseDTO();
    try {
      const session: Session_Schema = await this.sessionService.findByToken(token);
      if (!session) {
        response.messages.push("SESSION_NOT_FOUND");
        return response;
      }
      const company: string = session?.company ? "" + session.company : "";
      const config: CompanyConfig_Schema = await this.companyService.getCompanyConfig("" + company);
      response.setConfig(config);
    } catch (error) {
      response.messages.push(error?.message || "UNEXPECTED_ERROR_ON_SESSION_CONFIG_RETRIEVAL");

    }
    return response
  }

  @Get()
  @ApiQuery({
    name: 'status',
    required: false,
    type: String,
    enum: SessionStatus,
    description: 'Filter by status'
  })
  @RequireAuth({ guard: [JwtAuthGuard, RoleAuthGuard] })
  @AuthorizedRoles(...allRoles)
  @ApiResponse({ status: 200, type: GetAllSessionDTO })
  private async _getSessions(@AuthUser() user: User_Schema, @Query('status') status?: String): Promise<GetAllSessionDTO> {
    const response: GetAllSessionDTO = new GetAllSessionDTO()
    try {
      let filter: Partial<Session_Schema> = null;
      if (status && status.length > 0) {
        filter = { status: getSessionStatusRef(status as SessionStatus) };
      }
      const sessions: Session_Schema[] = await this.sessionService.findFiltered(new ObjectId(user["id"]), true, filter);
      response.init({ sessions, users: [user] });
      if (!response.result) response.messages.push("ERROR_ON_SESSION_RETRIEVAL");
    } catch (error) {
      response.messages.push(error?.message || "UNEXPECTED_ERROR_ON_SESSION_RETRIEVAL");
    }
    return response
  }
  @Get(':token/video')
  @RequireAuth({ guard: [JwtAuthGuard, RoleAuthGuard] })
  @ApiProperty({ type: String, name: "token" })
  @AuthorizedRoles(...allRoles)
  @ApiResponse({ status: 200, type: GetVideoURLDTO })
  private async _getVideo(@AuthUser() user: User_Schema, @Param('token') token: string): Promise<GetVideoURLDTO> {
    const response: GetVideoURLDTO = new GetVideoURLDTO()
    try {
      const videoUrl = await this.jService.getVideoURL(token)
      if (!videoUrl && !isNotEmpty(videoUrl) && videoUrl === 'undefined') {
        throw new Error(`INVALID URL: ${videoUrl}`)
      }
      response.result = true;
      response.videoViewURL = videoUrl
    } catch (error) {
      response.messages.push(error?.message || "UNEXPECTED_ERROR_ON_SESSION_RETRIEVAL");
    }
    return response
  }

  @Post()
  @RequireAuth({ guard: [JwtAuthGuard, RoleAuthGuard] })
  @AuthorizedRoles(...allRoles)
  @ApiBody({ type: PostSessionDTO })
  @ApiResponse({ status: 200, type: PostSessionResponseDTO })
  private async _createSession(@Body() body: PostSessionDTO, @AuthUser() user: User_Schema): Promise<PostSessionResponseDTO> {
    const response: PostSessionResponseDTO = new PostSessionResponseDTO();
    try {
      user._id = new ObjectId(user["id"]);
      const company: Company_Schema = await this.companyService.getUserCompany(user);
      const session: Session_Schema = await this.sessionService.createSession({ userId: new ObjectId(user._id), company: company._id, dto: body });
      const saved: boolean = await this.sessionService.save(session);
      if (!saved) {
        response.messages.push("ERROR_ON_SESSION_CREATION");
        return response;
      }
      response.init(session);
    } catch (error) {
      response.messages.push(error?.message || "UNEXPECTED_ERROR_ON_SESSION_CREATION");
    }
    return response
  }


  @Patch("/:token/status/pending")
  @ApiParam({ name: "token", type: String })
  @RequireAuth({ guard: [JwtAuthGuard, RoleAuthGuard] })
  @AuthorizedRoles(...allRoles)
  @ApiResponse({ status: 200, type: BaseAPIResponseDTO })
  private async _patchSession(@Param('token') token: string): Promise<BaseAPIResponseDTO> {
    const response: BaseAPIResponseDTO = new BaseAPIResponseDTO();
    try {
      const session = await this.sessionService.findByToken(token);
      if (!session._id) {
        response.result = false
        response.messages = ["TOKEN_NOT_FOUND"]
      }
      response.result = await this.sessionService.updateStatus(session, SessionStatusRef.PENDING);
    } catch (error) {
      response.result = false;
      response.messages.push(error?.message || "UNEXPECTED_ERROR_ON_SESSION_UPDATE");
    }
    return response
  }

  @Patch("/:token")
  @ApiParam({ name: "token", type: String })
  @ApiBody({ type: EditSessionDTO })
  @RequireAuth({ guard: [JwtAuthGuard, RoleAuthGuard] })
  @AuthorizedRoles(...allRoles)
  @ApiResponse({ status: 200, type: BaseAPIResponseDTO })
  @ApiOperation({ operationId: 'editSession' })
  private async editSession(@Param('token') token: string, @Body() body: EditSessionDTO): Promise<BaseAPIResponseDTO> {
    const response: BaseAPIResponseDTO = new BaseAPIResponseDTO();
    try {
      const session = await this.sessionService.findByToken(token);
      if (!session._id) {
        response.result = false
        response.messages = ["TOKEN_NOT_FOUND"]
      }
      response.result = await this.sessionService.update(session, { details: body.details, externalCode: body.extToken });
    } catch (error) {
      response.result = false;
      response.messages.push(error?.message || "UNEXPECTED_ERROR_ON_SESSION_UPDATE");
    }
    return response
  }

  @Delete("/:token")
  @RequireAuth({ guard: [JwtAuthGuard, RoleAuthGuard] })
  @ApiParam({ name: "token", type: String })
  @AuthorizedRoles(...managers)
  @ApiResponse({ status: 200, type: BaseAPIResponseDTO })
  private async deleteSession(@Param('token') token: string): Promise<BaseAPIResponseDTO> {
    const response: BaseAPIResponseDTO = new BaseAPIResponseDTO();
    try {
      const session = await this.sessionService.findByToken(token);
      if (!session._id) {
        response.result = false
        response.messages = ["TOKEN_NOT_FOUND"]
      }
      response.result = await this.sessionService.deleteSession(session);
    } catch (error) {
      response.result = false;
      response.messages.push(error?.message || "UNEXPECTED_ERROR_ON_SESSION_UPDATE");
    }
    return response
  }
}